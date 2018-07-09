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
    are required. For example 3 means only the date is required. If the requirement
    is not met, the result for that entry will be \code{NA}.}
}
%\details{
%}
\value{
  Numeric vector of the class \code{Date}. In fact this function
  computes the numeric vector and calls \code{.Date} to create the
  result.
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
  ## let us generate a bunch of random timestamps until today
  ts <- as.character(.Date(runif(1e4) * unclass(Sys.Date())))

  ## convert them using as.POSIXct
  system.time(a <- as.Date(ts))

  ## same using the fast method
  system.time(b <- fastDate(ts))

  identical(a, b)
}
