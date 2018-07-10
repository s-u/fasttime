fastDate <- function(x, required.components = 3L) {
    if( !is.character(x) ) {
        x <- as.character(x)
    }
    
    as.Date.numeric( .Call(parse_date, x, required.components),
                     origin = "1970-01-01" )

}

