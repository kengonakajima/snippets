require "logger"

logger = Logger.new( "/var/tmp/pooooo.log" )

logger.debug "ddddf\n"
logger.info "info\n"
logger.error "error\n"
logger.fatal "fatal\n"

logger.debug ["a","b","c"]

