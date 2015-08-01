
> This is a work in progress - some SVG files can be loaded, but many do not display correctly!


# SvgWin2D

Prototype for using [Win2D](https://github.com/Microsoft/Win2D) to render SVG images.

This is very much a work in progress and it isn't really ready for prime time yet.

There are two demo apps:

* TestSuite - this downloads the [SVG Test Suite](http://www.w3.org/Graphics/SVG/Test/20110816/harness/htmlObjectApproved/index.html) and displays the output from SvgWin2D next to the reference PNGs.  The tests are filtered to pick only the ones that have a chance of passing.
* SvgViewer - app that allows any SVG image to be loaded and displayed using SvgWin2D.

## Status

Many of the basic tests are doing roughly the right thing.  SVG files that use the individual style attributes (eg fill="red" rather than style="fill: red;") render something reasonable.  Many style attributes are not implemented yet.  Many entire feature areas are missing (eg filters, gradients, fonts, markers etc.).

However, if you do feel compelled to try this out and you spot something that doesn't work then please do feel free to file an issue - especially if you are able to attach an SVG file that doesn't work.  The larger the set of test images the better this can be.

Thank you for reading!
