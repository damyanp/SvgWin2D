
> This is a work in progress - not ready for any kind of consumption!


# SvgWin2D

Experiments with using [svgpp](https://github.com/svgpp/svgpp) and [Win2D](https://github.com/Microsoft/Win2D) to render SVG images.

This is very much a work in progress and it isn't really useful for anything yet.

Current version is able to pick up paths from an SVG file and turn them into a CanvasCommandList.  The paths are all filled black (all of the style values are ignored).

*Next steps:* after getting this far, I think that a better approach would be to have my own code drive the traversal of the SVG document, but then make use of SVG++'s value_parser to help parse the various mini-languages that SVG embeds in attributes, as suggested [here](http://stackoverflow.com/a/27657514).

Attempts to modify the [demo renderer](https://github.com/svgpp/svgpp/tree/master/src/demo/render) for Win2D ran into problems with VS running out of heap space (or ICEing).  There's also a mismatch between the very software rasterizer based approach in the demo renderer and the Win2D hardware accelerated approach that means that the changes to the demo are much more than adding a few #defines.
