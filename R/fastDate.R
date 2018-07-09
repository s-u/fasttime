fastDate <- function(x, required.components = 3L)
  .Date(if (is.character(x)) .Call(parse_date, x, required.components) else .Call(parse_date, as.character(x), required.components))
