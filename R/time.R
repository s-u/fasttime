fastPOSIXct <- function(x, tz=NULL, required.components = 3L, fixed=NA)
  if (is.character(x)) .Call(parse_ts, x, required.components, fixed, tz) else .Call(parse_ts, as.character(x), required.components, fixed, tz)

fastDate <- function(x, fixed=NA)
    fastPOSIXct(x, "Date", 3L, fixed=fixed)
