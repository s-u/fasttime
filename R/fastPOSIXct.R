
fastPOSIXct <- function(x, tz, required.components = 3L) {

    .POSIXct(
        xx = if( is.character( x ) ) {
            .Call(parse_ts, x, required.components)
        } else {
            .Call(parse_ts, as.character(x), required.components)
        },

        tz = if( missing(tz) ) {
            "UTC"
        } else {
            tz
        }
    )

}
