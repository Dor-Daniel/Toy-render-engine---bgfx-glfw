# save as png2c.py
from PIL import Image
import sys

if len(sys.argv) != 3:
    print("Usage: python png2c.py input.png output.h")
    sys.exit(1)

infile, outfile = sys.argv[1], sys.argv[2]

im = Image.open(infile).convert("RGBA")
w, h = im.size
pixels = list(im.getdata())

with open(outfile, "w") as f:
    f.write(f"// Generated from {infile}\n")
    f.write(f"#define ICON_WIDTH {w}\n")
    f.write(f"#define ICON_HEIGHT {h}\n")
    f.write("static unsigned char icon_pixels[] = {\n")
    for (r, g, b, a) in pixels:
        f.write(f" {r}, {g}, {b}, {a},")
    f.write("\n};\n")
