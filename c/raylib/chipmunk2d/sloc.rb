#!/usr/bin/env ruby
# frozen_string_literal: true

# Simple SLOC counter for this repository.
# - Counts non-blank, non-comment lines.
# - Supports C/C++-style (//, /* */) and hash-style (#) comments.
# - Scans current directory by default; pass a path as the first argument.
# - Excludes common dot-directories and debug/binary outputs.

require 'find'
require 'optparse'

# Language groups by comment style
C_STYLE_EXTS = %w[
  c cc cpp cxx h hpp hh inl inc m mm
  js jsx ts tsx java kt cs go rs swift php proto
].freeze

HASH_STYLE_EXTS = %w[
  rb py sh bash zsh fish
].freeze

# Special filenames (no extension) treated as code
SPECIAL_HASH_FILES = %w[
  Makefile Rakefile Gemfile
].freeze

DEFAULT_EXCLUDE_DIRS = [
  '.git', '.hg', '.svn', '.vscode', '.idea', '.venv',
  'node_modules', 'dist', 'build', 'out', 'target', 'tmp'
].freeze

def style_for(path)
  base = File.basename(path)
  return :hash if SPECIAL_HASH_FILES.include?(base)

  ext = File.extname(base).downcase.sub(/^\./, '')
  return :c if C_STYLE_EXTS.include?(ext)
  return :hash if HASH_STYLE_EXTS.include?(ext)

  nil
end

def text_lines(path)
  data = File.binread(path)
  text = data.encode('UTF-8', invalid: :replace, undef: :replace, replace: '')
  text.split(/\r\n|\r|\n/)
rescue => e
  warn "[warn] cannot read #{path}: #{e}"
  []
end

def count_sloc_c_style(lines)
  sloc = 0
  in_block = false

  lines.each do |line|
    sanitized = +''
    i = 0
    while i < line.length
      if in_block
        if line[i, 2] == '*/'
          in_block = false
          i += 2
        else
          i += 1
        end
      else
        if line[i, 2] == '/*'
          in_block = true
          i += 2
        elsif line[i, 2] == '//'
          break
        else
          sanitized << line[i]
          i += 1
        end
      end
    end

    sloc += 1 unless sanitized.strip.empty?
  end

  sloc
end

def count_sloc_hash_style(lines)
  preprocess_prefixes = %w[
    #if #ifdef #ifndef #elif #else #endif #define #undef
    #include #pragma #error #warning #line
  ]

  lines.count do |line|
    s = line.strip
    next false if s.empty?
    if s.start_with?('#')
      preprocess_prefixes.any? { |prefix| s.start_with?(prefix) }
    else
      true
    end
  end
end

def count_file(path, style)
  lines = text_lines(path)
  case style
  when :c
    count_sloc_c_style(lines)
  when :hash
    count_sloc_hash_style(lines)
  else
    0
  end
end

def excluded_dir?(path)
  base = File.basename(path)
  return true if base.start_with?('.') && base != '.' && base != '..'
  return true if DEFAULT_EXCLUDE_DIRS.include?(base)
  return true if base.end_with?('.dSYM')
  false
end

options = {
  root: ARGV[0] || '.',
  details: true,
  list_files: true,
}

OptionParser.new do |opt|
  opt.banner = 'Usage: ruby sloc.rb [PATH]'
  opt.on('--no-details', 'Do not show per-extension breakdown') { options[:details] = false }
  opt.on('--no-files', 'Do not show per-file ranking') { options[:list_files] = false }
end.parse!(ARGV[1..] || [])

root = options[:root]
unless File.directory?(root)
  warn "[error] not a directory: #{root}"
  exit 1
end

per_ext = Hash.new { |h, k| h[k] = { files: 0, sloc: 0 } }
total_files = 0
total_sloc = 0
files = []

Find.find(root) do |path|
  if File.directory?(path)
    if excluded_dir?(path)
      Find.prune
    end
    next
  end

  st = style_for(path)
  next unless st

  sloc = count_file(path, st)
  next if sloc <= 0

  ext = File.extname(path).downcase.sub(/^\./, '')
  ext = 'no-ext' if ext.empty?
  per_ext[ext][:files] += 1
  per_ext[ext][:sloc] += sloc
  total_files += 1
  total_sloc += sloc
  files << [path, sloc]
end

if options[:details]
  # Sort by SLOC desc
  width = per_ext.keys.map(&:length).max.to_i
  puts 'SLOC by extension:'
  per_ext.sort_by { |_k, v| -v[:sloc] }.each do |ext, v|
    puts format("  %-#{[width, 3].max}s  files:%5d  sloc:%8d", ext, v[:files], v[:sloc])
  end
  puts
end

puts format('Total files: %d', total_files)
puts format('Total SLOC : %d', total_sloc)

if options[:list_files]
  puts
  puts 'SLOC by file (desc):'
  files.sort_by { |(_p, s)| -s }.each do |path, sloc|
    puts format('%8d  %s', sloc, path)
  end
end
