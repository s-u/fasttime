fastPOSIXct <- function(x, tz=NULL, required.components = 3L)
  .POSIXct(if (is.character(x)) .Call("parse_ts", x, required.components) else .Call("parse_ts", as.character(x), required.components), tz)
