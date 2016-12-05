************
*Installing*
************
Copy this folder to your KiCAD 3Dshapes-folder to view the 3D models.
You can find the default folder of KiCADs 3D models when editing 3D settings 
of footprints.

************************
*Making your own models*
************************
KiCAD understands VRML (.wrl) 3D models. Create the 3D model in your favourite STL-export
capable 3D CAD, and then run "meshconv.exe -c wrl <inputfile> -o <outputfile>" to convert 
STL into WRL.

3D model then has to be installed to KiCAD 3D model folder, see "Installing" for details.
Finally KiCAD footprint has to be setup to use the model my editing 3D settings of the part.
If there is a scaling issue, try scaling the model to/from inches.

*********
Licenses*
*********
Meshconv is written by Patric Min, it is available at http://www.cs.princeton.edu/~min/meshconv/ .  