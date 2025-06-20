# FoodRecApp Planning Document

## Overview

This project aims to create an application that automatically reads the "Best-By" date on groceries using image detection and stores the data in a database. The application should also remind the user before items expire and suggest meal ideas using a language model based on the items nearing expiration. The implementation is planned in C to gain experience with the language.

## Core Features

1. **Image Detection for Date Recognition**
   - Capture images of grocery packaging via camera or user upload.
   - Use an OCR or image detection library (e.g., Tesseract or OpenCV) to recognize the printed "Best-By" dates.
   - Parse recognized text into a consistent date format.

2. **Database Storage**
   - Design a small SQL or embedded database (e.g., SQLite) to store item names, best-by dates, and quantities.
   - Sort or query items by date to determine which items expire soon.

3. **Reminder Service**
   - Periodically check the database for items nearing expiration.
   - Notify the user via a command-line message, email, or system notification depending on the environment.

4. **Recipe Suggestions Using LLM**
   - When items are close to expiry, generate a list of those items.
   - Send the list to an LLM such as OpenAI's API to obtain recipe ideas.
   - Present the recipe suggestions to the user.

## High-Level Design in C

- **Data Model**
  - Create a `struct GroceryItem` to hold item name, best-by date (as `time_t`), and quantity.
  - Use SQLite through its C API for persistence.

- **Image Processing**
  - Integrate OpenCV or another image processing library for reading images.
  - Use Tesseract OCR (via its C API) to extract text from the images.
  - Implement error checking to ensure dates are read correctly.

- **LLM Interaction**
  - Make HTTP requests from C using a networking library (e.g., libcurl).
  - Format the request with the items nearing expiration and parse the response for recipe suggestions.

- **Scheduling and Notifications**
  - Create a simple scheduler in C that checks for items expiring within a threshold (e.g., 3 days).
  - Use standard console output for reminders initially; integrate with other notification systems if needed.

## Project Roadmap

1. **Set Up the Environment**
   - Choose or build a small command-line interface in C.
   - Install dependencies (OpenCV, Tesseract, SQLite, libcurl).

2. **Implement Database Layer**
   - Define database schema.
   - Write functions to add, update, and query grocery items.

3. **Implement Image Detection**
   - Capture or load images containing best-by dates.
   - Process images and extract date text.
   - Insert recognized data into the database.

4. **Implement Reminder Logic**
   - Create routines to check for items nearing expiration.
   - Print or send notifications to the user.

5. **Connect to LLM for Recipe Suggestions**
   - Build a request with expiring items.
   - Use libcurl to query OpenAI or another API.
   - Display recipe results.

6. **Testing and Refinement**
   - Add sample images for testing image detection.
   - Ensure database queries are reliable.
   - Test the entire workflow from image capture to recipe suggestion.

## Learning Goals

- Practice writing C programs that interact with external libraries and APIs.
- Gain experience with image recognition, database manipulation, and HTTP requests in C.
- Experiment with integrating an LLM to generate helpful meal ideas from expiring groceries.

