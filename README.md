# Library Management System

A comprehensive C++ library management system with user roles, book management, and fine calculation.

## Features

### User Roles
1. **Students**
   - Can borrow up to 3 books
   - 15-day borrowing period
   - Fine rate: Rs. 10 per day overdue

2. **Faculty**
   - Can borrow up to 5 books
   - 30-day borrowing period
   - No fines for overdue books

3. **Librarian**
   - Full system access
   - Cannot borrow books
   - Can manage books and users
   - Can view all borrowed books

### Book Management
- Search books by title/author
- View all books
- Book status tracking:
  - Available: Can be borrowed by anyone
  - Reserved: Only available to the person who reserved it
  - Borrowed: Currently checked out

### Reservation System
- Users can reserve borrowed books
- First-come-first-served queue system
- Automatic notification when reserved book is returned
- Users can cancel their reservations

### Fine Management
- Automatic fine calculation
- View outstanding fines
- Pay fines
- Borrowing blocked if fines are pending

## Test Accounts

### Students (3 books max)
- ID: 111  Password: test123
- ID: 112  Password: test456

### Faculty (5 books max)
- ID: 211  Password: tfac123
- ID: 212  Password: tfac456

### Librarian (full access)
- ID: 306  Password: tlib123

## Project Structure 

```
Library Management System/
├── main.cpp                 # Main program entry point
├── header/                  # Header files
│   └── LibrarySystem.h     # Main header file with class declarations
├── src/                    # Source files
│   └── LibrarySystem.cpp   # Implementation of library system classes
└── data/                   # Data storage directory
    ├── books.txt          # Book information
    ├── students.txt       # Student user data
    ├── faculty.txt        # Faculty user data
    ├── librarians.txt     # Librarian user data
    └── accounts/          # User account data
        └── *.txt          # Individual account files
```

### Data File Formats

1. books.txt:
```
bookID|title|author|publisher|year|ISBN|availability
```

2. users.txt (students/faculty/librarians):
```
userID|name|password|department
```

3. accounts/[userID].txt:
```
BORROW|bookID|borrowDate|dueDate
HISTORY|bookID|borrowDate|dueDate
FINE|amount
```

## Usage

1. Login using provided test accounts
2. Navigate using the menu system:
   - Search and view books
   - Borrow/return books
   - Make/cancel reservations
   - View/pay fines
   - Manage books (Librarian)
   - Manage users (Librarian)

## Error Handling
- Invalid credentials
- Book not found
- User not found
- Borrowing limit reached
- Outstanding fines
- Invalid reservations
- Unauthorized access

## Data Persistence
- All data is automatically saved after each operation
- Book status, user records, and fines are maintained between sessions
- Borrowing history is preserved

## How to Compile and Run

### Prerequisites
- G++ compiler
- C++11 or higher

### Compilation
Open terminal/command prompt in the project root directory and run:
```bash
g++ main.cpp src/*.cpp -o main
```

### Running the Program
After compilation:
- On Windows:
  ```bash
  main
  ```
- On Linux/Mac:
  ```bash
  ./main
  ```

## User Privileges

### Students
- Search books
- View all books
- View borrowed books
- Borrow books (max 3)
- Return books
- Reserve unavailable books
- Cancel reservations
- View reservations
- View fines
- Pay fines

### Faculty
- Search books
- View all books
- View borrowed books
- Borrow books (max 5)
- Return books
- Reserve unavailable books
- Cancel reservations
- View reservations
- View fines
- Pay fines

### Librarian
- Add new books
- Remove books
- Add new users
- Remove users
- Check user details
- View all borrowed books
- Search books
- View all books

## Notes

- The system uses file-based storage for persistence
- Fines are calculated based on user type and overdue duration
- Books can be searched by title or author
- Each user type has different borrowing limits and privileges
- Reservations are automatically processed when books are returned
- Account data is stored in separate files for each user

## Troubleshooting

1. If compilation fails:
   - Ensure all source files are in the correct directories
   - Check if G++ is installed and properly configured
   - Make sure you're using C++11 or higher

2. If program doesn't start:
   - Verify that all data files exist in the data/ directory
   - Check file permissions
   - Ensure the data/accounts directory exists

3. If login fails:
   - Verify you're using correct credentials from the test accounts
   - Check if user data files are properly formatted