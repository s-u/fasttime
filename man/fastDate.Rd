\name{fastDate}
\alias{fastDate}
\title{
  Fast version of \code{as.Date.character}.
}
\description{
  It is extremely fast (compared to \code{as.Date} by several orders
  of magnitude since it uses pure text parsing and no system calls.
}
\usage{
fastDate(x, required.components = 3L)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{x}{string vector to interpret as dates}
  \item{required.components}{minimum number of timestamp components that
    are required. For example 2 means only the year and month are required.
    If the requirement is not met, the result for that entry will be \code{NA}.}
}
%\details{
%}
\value{
  Numeric vector of the class \code{Date}. In fact this function
  computes the numeric vector and calls \code{as.Date.numeric} to
  create the result.
}
%\references{
%}
\author{
  Simon Urbanek
}
\seealso{
  \code{\link{as.Date}}
}
\examples{
  ## generate a list of every date from 1901-2500
  x <- as.integer(as.Date(c("1901-01-01","2500-12-31")))
  x <- as.character(as.Date(seq.int(x[1],x[2],1L),origin="1970-01-01"))
  
  ## convert them using as.Date
  system.time(a <- as.Date(x))

  ## same using the fast method
  system.time(b <- fastDate(x))

  identical(a, b)
}
