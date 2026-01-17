#!/usr/bin/env node

const fs = require('fs/promises');
const path = require('path');
const { Marp } = require('@marp-team/marp-core');

const defaultInput = 'a.md';

async function main() {
  const [, , inputArg, outputArg] = process.argv;
  const inputPath = inputArg ?? defaultInput;
  const resolvedInputPath = path.resolve(process.cwd(), inputPath);
  const resolvedOutputPath = path.resolve(
    process.cwd(),
    outputArg ?? replaceExtension(inputPath, '.html')
  );

  const markdown = await fs.readFile(resolvedInputPath, 'utf8');
  const marp = new Marp({
    html: true,
  });
  const { html, css } = marp.render(markdown);

  const page = assembleHtml(css, html);
  await fs.writeFile(resolvedOutputPath, page, 'utf8');

  console.log(`✨ ${inputPath} -> ${resolvedOutputPath}`);
}

function replaceExtension(filename, extension) {
  const ext = path.extname(filename);
  if (!ext) return `${filename}${extension}`;
  return `${filename.slice(0, -ext.length)}${extension}`;
}

function assembleHtml(css, body) {
  const presentationCss = `
body {
  margin: 0;
  background-color: #111;
  font-family: "Hiragino Kaku Gothic ProN", "Meiryo", "Segoe UI", sans-serif;
  overflow: hidden;
}
:root {
  color-scheme: light;
}
.marpit-wrapper {
  position: fixed;
  inset: 0;
  display: flex;
  align-items: center;
  justify-content: center;
}
.marpit {
  position: relative;
  width: 1280px;
  height: 720px;
  transform-origin: center center;
  box-shadow: 0 24px 60px rgba(0, 0, 0, 0.45);
  border-radius: 18px;
  overflow: hidden;
  background: #000;
}
.marpit > svg {
  display: none;
}
.marpit > svg[data-active="true"] {
  display: block;
}
.page-indicator {
  position: fixed;
  left: 24px;
  bottom: 16px;
  font-size: 0.85rem;
  color: rgba(255, 255, 255, 0.75);
  letter-spacing: 0.08em;
}
.navigation-hint {
  position: fixed;
  right: 24px;
  bottom: 16px;
  font-size: 0.8rem;
  color: rgba(255, 255, 255, 0.7);
  font-family: "Hiragino Kaku Gothic ProN", "Meiryo", sans-serif;
}
`;

  const navigationScript = `
(function () {
  const marpit = document.querySelector('.marpit');
  if (!marpit) return;

  const slides = Array.from(marpit.querySelectorAll(':scope > svg'));
  if (slides.length === 0) return;

  const total = slides.length;
  const indicator = document.querySelector('.page-indicator');
  let index = 0;

  slides.forEach((svg) => svg.setAttribute('data-active', 'false'));

  function updateIndicator() {
    if (indicator) indicator.textContent = \`\${index + 1} / \${total}\`;
  }

  function focus(newIndex) {
    const next = Math.max(0, Math.min(total - 1, newIndex));
    slides[index]?.setAttribute('data-active', 'false');
    index = next;
    slides[index].setAttribute('data-active', 'true');
    updateIndicator();
  }

  function next() {
    if (index < total - 1) focus(index + 1);
  }

  function prev() {
    if (index > 0) focus(index - 1);
  }

  function updateScale() {
    const baseWidth = 1280;
    const baseHeight = 720;
    const scale = Math.min(
      window.innerWidth / baseWidth,
      window.innerHeight / baseHeight
    );
    marpit.style.transform = \`translate(-50%, -50%) scale(\${scale})\`;
    marpit.style.left = '50%';
    marpit.style.top = '50%';
    marpit.style.position = 'fixed';
  }

  document.addEventListener('keydown', (event) => {
    const { key } = event;
    if (['ArrowRight', 'ArrowDown', 'PageDown', ' '].includes(key)) {
      event.preventDefault();
      next();
    } else if (['ArrowLeft', 'ArrowUp', 'PageUp'].includes(key)) {
      event.preventDefault();
      prev();
    } else if (key === 'Home') {
      event.preventDefault();
      focus(0);
    } else if (key === 'End') {
      event.preventDefault();
      focus(total - 1);
    }
  });

  document.addEventListener('click', (event) => {
    if (event.target.closest('.marpit')) {
      next();
    }
  });

  document.addEventListener(
    'wheel',
    (event) => {
      if (Math.abs(event.deltaY) >= Math.abs(event.deltaX)) {
        event.preventDefault();
        if (event.deltaY > 0) {
          next();
        } else if (event.deltaY < 0) {
          prev();
        }
      }
    },
    { passive: false }
  );

  window.addEventListener('resize', updateScale);

  focus(0);
  updateScale();
})();
`;

  return `<!DOCTYPE html>
<html lang="ja">
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Marp Slide</title>
    <style>
${css}
${presentationCss}
    </style>
  </head>
  <body>
    <div class="marpit-wrapper">
${body}
    </div>
    <div class="page-indicator"></div>
    <div class="navigation-hint">← / → キーで移動</div>
    <script>
${navigationScript}
    </script>
  </body>
</html>
`;
}


main();
