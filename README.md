# FoodRecApp

This is a minimal command-line application for tracking groceries and their
expiration dates. Items can be added manually with a best-by date or scanned
from an image using Tesseract OCR. Data is stored in an SQLite database.
The OCR feature now recognises many common date formats (e.g.
`YYYY-MM-DD`, `YYYY/MM/DD`, `MM/DD/YY`, `Jan 3 2025`, `Feb 3, 25`).
Set `FOODREC_DEBUG=1` to print the raw OCR text for troubleshooting.

## Building

```
make
```

Requires GCC, SQLite development libraries, and Tesseract OCR for the optional
`scan` command. Ensure the `tesseract` binary is available in your `PATH` when
running the program.

## Usage

```
./foodrec add <name> <YYYY-MM-DD>   # add an item manually
./foodrec scan <image> <name>       # OCR an image for a date and add item
./foodrec list                      # list all items in the database
./foodrec check                     # show items expiring in the next 3 days
./foodrec suggest                   # placeholder for recipe suggestions
```

The database file `foodrec.db` is created in the working directory on first run.
