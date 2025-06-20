# FoodRecApp

This is a minimal command-line application for tracking groceries and their
expiration dates. Items can be added manually with a best-by date or scanned
from an image using Tesseract OCR. Data is stored in an SQLite database.

## Building

```
make
```

Requires GCC, SQLite development libraries, and Tesseract OCR for the optional
`scan` command.

## Usage

```
./foodrec add <name> <YYYY-MM-DD>   # add an item manually
./foodrec scan <image> <name>       # OCR an image for a date and add item
./foodrec list                      # list all items in the database
./foodrec check                     # show items expiring in the next 3 days
./foodrec suggest                   # placeholder for recipe suggestions
```

The database file `foodrec.db` is created in the working directory on first run.
