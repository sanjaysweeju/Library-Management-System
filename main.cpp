#include <iostream>
#include <memory>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <functional>
#include <vector>
#include "header/LibrarySystem.h"

using namespace std;

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to read data from a file
void readDataFile(const std::string& filename, std::function<void(const std::vector<std::string>&)> processor) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        auto parts = split(line, '|');
        processor(parts);
    }
}

// Function declarations
void clearInputBuffer();
void waitForEnter();
void displayMenu();
void displayUserMenu(const User* user);
void displayBookDetails(const Book* book);
void handleSearchBooks(const Library& library);
void handleBorrowBook(Library& library, int userID);
void handleReturnBook(Library& library, int userID);
void handleViewFines(const Library& library, int userID);
void handlePayFine(Library& library, int userID);
void handleAddBook(Library& library);
void handleRemoveBook(Library& library);
void handleAddUser(Library& library);
void handleRemoveUser(Library& library);
void handleCheckUser(const Library& library);
void handleViewAllBooks(const Library& library);
void handleReserveBook(Library& library, int userID);
void handleCancelReservation(Library& library, int userID);
void handleViewReservations(const Library& library, int userID);
void handleViewAllBorrowedBooks(const Library& library);
void initializeLibrary(Library& lib);

void displayMenu() {
    cout << "\n\n";
    cout << "    Library Management System\n\n\n";
    cout << "1. Login\n";
    cout << "2. Exit\n\n\n";
    cout << "Test Accounts:\n\n";
    cout << "Students (3 books max):\n";
    cout << "  ID: 111  Password: test123\n";
    cout << "  ID: 112  Password: test456\n\n";
    cout << "Faculty (5 books max):\n";
    cout << "  ID: 211  Password: tfac123\n";
    cout << "  ID: 212  Password: tfac456\n\n";
    cout << "Librarian (full access):\n";
    cout << "  ID: 306  Password: tlib123\n\n\n";
    cout << "Enter choice (1-2): ";
}

void displayUserMenu(const User* user) {
    cout << "\n\n";
    cout << "Welcome " << user->getName() << "\n";
    cout << "Role: " << user->getRole() << "\n\n\n";
    cout << "1. Search Books\n";
    cout << "2. View All Books\n";
    
    if (user->canBorrow()) {
        cout << "3. Borrow Book\n";
        cout << "4. Return Book\n";
        cout << "5. Reserve Book\n";
        cout << "6. Cancel Reservation\n";
        cout << "7. View Reservations\n";
        cout << "8. View Borrowed Books\n";
        cout << "9. View Fines\n";
        cout << "10. Pay Fine\n";
    }
    
    if (user->canManageBooks()) {
        cout << "11. Add Book\n";
        cout << "12. Remove Book\n";
    }
    
    if (user->canManageUsers()) {
        cout << "13. Add User\n";
        cout << "14. Remove User\n";
        cout << "15. Check User\n";
        cout << "16. View All Borrowed Books\n";
    }
    
    cout << "\n0. Logout\n";
    cout << "\nEnter choice: ";
}

void displayBookDetails(const Book* book) {
    if (!book) return;
    cout << "\nBook Details:\n";
    cout << "ID: " << book->getBookID() << "\n";
    cout << "Title: " << book->getTitle() << "\n";
    cout << "Author: " << book->getAuthor() << "\n";
    cout << "Publisher: " << book->getPublisher() << "\n";
    cout << "Year: " << book->getYear() << "\n";
    cout << "ISBN: " << book->getISBN() << "\n";
    cout << "Status: ";
    if (book->isAvailable()) {
        if (book->isReserved()) {
            cout << "Reserved";
        } else {
            cout << "Available";
        }
    } else {
        cout << "Borrowed";
    }
    cout << "\n";
}

void handleSearchBooks(const Library& library) {
    clearInputBuffer();
    string query;
    cout << "Enter search term (title/author): ";
    getline(cin, query);

    auto results = library.searchBooks(query);
    if (results.empty()) {
        cout << "No books found.\n";
        return;
    }

    cout << "\nFound " << results.size() << " books:\n";
    for (const auto* book : results) {
        displayBookDetails(book);
    }
}

void handleBorrowBook(Library& library, int userID) {
    int bookID;
    cout << "Enter Book ID to borrow: ";
    cin >> bookID;
    cin.ignore();

    const Book* book = library.getBook(bookID);
    if (!book) {
        cout << "Error: Book not found.\n";
        return;
    }

    const User* user = library.getUser(userID);
    if (!user) {
        cout << "Error: User not found.\n";
        return;
    }

    Account* account = library.getAccount(userID);
    if (!account) {
        cout << "Error: Account not found.\n";
        return;
    }

    // Check if user can borrow (not a librarian)
    if (!user->canBorrow()) {
        cout << "Error: Your role (" << user->getRole() << ") is not allowed to borrow books.\n";
        return;
    }

    // Check if book is available for this user
    if (!book->isAvailableFor(userID)) {
        if (book->isReserved()) {
            if (book->isReservedBy(userID)) {
                cout << "You have already reserved this book but it's not yet available.\n";
            } else {
                cout << "Error: This book is reserved by another user.\n";
            }
        } else {
            cout << "Error: This book is currently borrowed.\n";
            cout << "You can reserve it for when it becomes available.\n";
        }
        return;
    }

    // Check borrowing limit
    if (account->getCurrentBorrows().size() >= user->getMaxBooks()) {
        cout << "Error: You have reached your borrowing limit of " 
             << user->getMaxBooks() << " books.\n";
        return;
    }

    // Check if user already has this book
    for (const auto& borrow : account->getCurrentBorrows()) {
        if (borrow.bookID == bookID) {
            cout << "Error: You have already borrowed this book.\n";
            return;
        }
    }

    // Check for outstanding fines
    if (account->getTotalFine() > 0) {
        cout << "Error: You have outstanding fines of Rs. " 
             << fixed << setprecision(2) << account->getTotalFine() 
             << ". Please pay your fines before borrowing.\n";
        return;
    }

    if (library.borrowBook(userID, bookID)) {
        cout << "Book borrowed successfully!\n";
        const auto& borrows = account->getCurrentBorrows();
        time_t dueTime = chrono::system_clock::to_time_t(borrows.back().dueDate);
        cout << "Due date: " << ctime(&dueTime);
    } else {
        cout << "Error: Failed to borrow book. Please try again.\n";
    }
}

void handleReturnBook(Library& library, int userID) {
    int bookID;
    cout << "Enter Book ID to return: ";
    cin >> bookID;
    cin.ignore();

    const Book* book = library.getBook(bookID);
    if (!book) {
        cout << "Error: Book not found.\n";
        return;
    }

    const User* user = library.getUser(userID);
    if (!user) {
        cout << "Error: User not found.\n";
        return;
    }

    Account* account = library.getAccount(userID);
    if (!account) {
        cout << "Error: Account not found.\n";
        return;
    }

    // Check if user has borrowed this book
    bool hasBorrowed = false;
    for (const auto& borrow : account->getCurrentBorrows()) {
        if (borrow.bookID == bookID) {
            hasBorrowed = true;
            break;
        }
    }

    if (!hasBorrowed) {
        cout << "Error: You have not borrowed this book.\n";
        return;
    }

    if (library.returnBook(userID, bookID)) {
        cout << "Book returned successfully!\n";
        
        // Check for fines after return
        if (account->getTotalFine() > 0) {
            cout << "Fine due: Rs. " << fixed << setprecision(2) 
                     << account->getTotalFine() << "\n";
            cout << "Please pay your fines to avoid restrictions on future borrowings.\n";
        }
    } else {
        cout << "Error: Failed to return book. Please try again.\n";
    }
}

void handleViewFines(const Library& library, int userID) {
    const Account* account = library.getAccount(userID);
    if (account) {
        double fine = account->getTotalFine();
        if (fine > 0) {
            cout << "Total fine: Rs. " << fixed << setprecision(2) << fine << "\n";
        } else {
            cout << "No outstanding fines.\n";
        }
    }
}

void handlePayFine(Library& library, int userID) {
    const Account* account = library.getAccount(userID);
    if (!account || account->getTotalFine() == 0) {
        cout << "No fines to pay.\n";
        return;
    }

    double amount;
    cout << "Total fine: Rs. " << fixed << setprecision(2) 
              << account->getTotalFine() << "\n";
    cout << "Enter amount to pay: ";
    cin >> amount;

    if (library.payFine(userID, amount)) {
        cout << "Payment successful!\n";
    } else {
        cout << "Payment failed.\n";
    }
}

void handleViewBorrowedBooks(const Library& library, int userID) {
    const Account* account = library.getAccount(userID);
    if (!account || account->getCurrentBorrows().empty()) {
        cout << "No borrowed books.\n";
        return;
    }

    cout << "\nBorrowed Books:\n";
    for (const auto& record : account->getCurrentBorrows()) {
        const Book* book = library.getBook(record.bookID);
        if (book) {
            displayBookDetails(book);
        }
    }
}

void handleAddBook(Library& library) {
    int bookID;
    string title, author, publisher, isbn;
    int year;

    cout << "\nAdd New Book\n";
    cout << "Enter Book ID: ";
    cin >> bookID;
    clearInputBuffer();

    cout << "Enter Title: ";
    getline(cin, title);

    cout << "Enter Author: ";
    getline(cin, author);

    cout << "Enter Publisher: ";
    getline(cin, publisher);

    cout << "Enter Year: ";
    cin >> year;
    clearInputBuffer();

    cout << "Enter ISBN: ";
    getline(cin, isbn);

    auto book = std::make_unique<Book>(bookID, title, author, publisher, year, isbn);
    if (library.addBook(std::move(book))) {
        cout << "Book added successfully!\n";
    } else {
        cout << "Failed to add book.\n";
    }
}

void handleRemoveBook(Library& library) {
    int bookID;
    cout << "Enter Book ID to remove: ";
    cin >> bookID;

    if (library.removeBook(bookID)) {
        cout << "Book removed successfully!\n";
    } else {
        cout << "Failed to remove book.\n";
    }
}

void handleAddUser(Library& library) {
    int userID;
    string name, password, department;
    char userType;

    cout << "\nAdd New User\n";
    cout << "User Type (S/F/L): ";
    cin >> userType;
    cin.ignore();

    cout << "Enter ID: ";
    cin >> userID;
    cin.ignore();

    cout << "Enter Name: ";
    getline(cin, name);

    cout << "Enter Password: ";
    getline(cin, password);

    cout << "Enter Department: ";
    getline(cin, department);

    std::unique_ptr<User> user;
    switch (std::toupper(userType)) {
        case 'S':
            user = std::make_unique<Student>(userID, name, password);
            break;
        case 'F':
            user = std::make_unique<Faculty>(userID, name, password);
            break;
        case 'L':
            user = std::make_unique<Librarian>(userID, name, password);
            break;
        default:
            cout << "Invalid user type!\n";
            return;
    }

    user->setDepartment(department);
    if (library.addUser(std::move(user))) {
        cout << "User added successfully!\n";
    } else {
        cout << "Failed to add user.\n";
    }
}

void handleRemoveUser(Library& library) {
    int userID;
    cout << "Enter User ID to remove: ";
    cin >> userID;

    if (library.removeUser(userID)) {
        cout << "User removed successfully!\n";
    } else {
        cout << "Failed to remove user.\n";
    }
}

void handleCheckUser(const Library& library) {
    int userID;
    cout << "Enter User ID to check: ";
    cin >> userID;

    const User* user = library.getUser(userID);
    if (user) {
        cout << "\nUser Details:\n";
        cout << "ID: " << user->getUserID() << "\n";
        cout << "Name: " << user->getName() << "\n";
        cout << "Role: " << user->getRole() << "\n";
        cout << "Department: " << user->getDepartment() << "\n";
    } else {
        cout << "User not found.\n";
    }
}

void handleViewAllBooks(const Library& library) {
    cout << "\n=== All Books in Library ===\n";
    
    auto books = library.searchBooks("");  // Empty string to get all books
    if (books.empty()) {
        cout << "No books in the library.\n";
        return;
    }

    cout << "\nTotal Books: " << books.size() << "\n";
    cout << "--------------------\n";
    
    for (const auto* book : books) {
        displayBookDetails(book);
        cout << "--------------------\n";
    }
}

void handleReserveBook(Library& library, int userID) {
    int bookID;
    cout << "Enter Book ID to reserve: ";
    cin >> bookID;
    cin.ignore();

    const Book* book = library.getBook(bookID);
    if (!book) {
        cout << "Error: Book not found.\n";
        return;
    }

    if (book->isReservedBy(userID)) {
        cout << "Error: You already have a reservation for this book.\n";
        return;
    }

    if (book->isAvailable()) {
        cout << "Error: This book is currently available. You can borrow it directly.\n";
        return;
    }

    if (library.reserveBook(userID, bookID)) {
        cout << "Book reserved successfully!\n";
    } else {
        cout << "Error: Failed to reserve book. Please try again.\n";
    }
}

void handleCancelReservation(Library& library, int userID) {
    int bookID;
    cout << "Enter Book ID to cancel reservation: ";
    cin >> bookID;
    cin.ignore();

    const Book* book = library.getBook(bookID);
    if (!book) {
        cout << "Error: Book not found.\n";
        return;
    }

    if (book->isReservedBy(userID)) {
        if (library.cancelReservation(userID, bookID)) {
            cout << "Reservation cancelled successfully!\n";
        } else {
            cout << "Error: Failed to cancel reservation. Please try again.\n";
        }
    } else {
        cout << "Error: You don't have a reservation for this book.\n";
    }
}

void handleViewReservations(const Library& library, int userID) {
    auto books = library.getReservedBooks(userID);
    if (books.empty()) {
        cout << "You have no book reservations.\n";
        return;
    }

    cout << "\nYour Reserved Books:\n";
    cout << "--------------------\n";
    for (const auto* book : books) {
        displayBookDetails(book);
        cout << "--------------------\n";
    }
}

void handleViewAllBorrowedBooks(const Library& library) {
    auto borrowedBooks = library.getAllBorrowedBooks();
    if (borrowedBooks.empty()) {
        cout << "No books are currently borrowed.\n";
        return;
    }

    cout << "\n=== Currently Borrowed Books ===\n\n";
    for (const auto& info : borrowedBooks) {
        cout << "Book Details:\n";
        cout << "-------------\n";
        displayBookDetails(info.book);
        cout << "\nBorrower Details:\n";
        cout << "----------------\n";
        cout << "ID: " << info.borrower->getUserID() << "\n";
        cout << "Name: " << info.borrower->getName() << "\n";
        cout << "Role: " << info.borrower->getRole() << "\n";
        cout << "Department: " << info.borrower->getDepartment() << "\n";
        
        // Convert time_points to readable format
        auto borrowTime = chrono::system_clock::to_time_t(info.borrowDate);
        auto dueTime = chrono::system_clock::to_time_t(info.dueDate);
        
        cout << "\nBorrow Date: " << ctime(&borrowTime);
        cout << "Due Date: " << ctime(&dueTime);
        cout << "============================\n\n";
    }
}

// Add these function definitions right after your includes and before other functions

void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void waitForEnter() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Function to initialize the library with data from files
void initializeLibrary(Library& lib) {
    // Read books
    readDataFile("data/books.txt", [&lib](const auto& parts) {
        if (parts.size() == 7) {  
            int id = std::stoi(parts[0]);
            int year = std::stoi(parts[4]);
            bool available = parts[6] == "1";
            auto book = std::make_unique<Book>(
                id, parts[1], parts[2], parts[3], year, parts[5]
            );
            book->setAvailable(available);  // Set the availability status
            lib.addBook(std::move(book));
        }
    });

    // Read students
    readDataFile("data/students.txt", [&lib](const auto& parts) {
        if (parts.size() == 4) {
            int id = std::stoi(parts[0]);
            auto student = std::make_unique<Student>(id, parts[1], parts[2]);
            student->setDepartment(parts[3]);
            lib.addUser(std::move(student));
            lib.loadAccountInfo(id);  // Load account information
        }
    });

    // Read faculty
    readDataFile("data/faculty.txt", [&lib](const auto& parts) {
        if (parts.size() == 4) {
            int id = std::stoi(parts[0]);
            auto faculty = std::make_unique<Faculty>(id, parts[1], parts[2]);
            faculty->setDepartment(parts[3]);
            lib.addUser(std::move(faculty));
            lib.loadAccountInfo(id);  // Load account information
        }
    });

    // Read librarians
    readDataFile("data/librarians.txt", [&lib](const auto& parts) {
        if (parts.size() == 4) {
            int id = std::stoi(parts[0]);
            auto librarian = std::make_unique<Librarian>(id, parts[1], parts[2]);
            librarian->setDepartment(parts[3]);
            lib.addUser(std::move(librarian));
            lib.loadAccountInfo(id);  // Load account information
        }
    });
}

int main() {
    Library library;
    initializeLibrary(library);

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;

        if (choice == 2) {
            library.saveState();  // Save before exiting
            cout << "Thank you for using the Library Management System!\n";
            waitForEnter();
            break;
        }

        if (choice == 1) {
            int userID;
            string password;
            cout << "Enter User ID: ";
            cin >> userID;
            cout << "Enter Password: ";
            cin >> password;

            if (library.authenticateUser(userID, password)) {
                const User* user = library.getUser(userID);
                if (user) {
                    while (true) {
                        displayUserMenu(user);
                        int userChoice;
                        cin >> userChoice;

                        if (userChoice == 0) {
                            library.saveState();  // Save before logging out
                            cout << "Logging out...\n";
                            waitForEnter();
                            break;
                        }

                        switch (userChoice) {
                            case 1: 
                                handleSearchBooks(library); 
                                waitForEnter();
                                break;
                            case 2:
                                handleViewAllBooks(library);
                                waitForEnter();
                                break;
                            case 3:
                                if (user->canBorrow()) {
                                    handleBorrowBook(library, userID);
                                    library.saveState();
                                    waitForEnter();
                                }
                                break;
                            case 4:
                                if (user->canBorrow()) {
                                    handleReturnBook(library, userID);
                                    library.saveState();
                                    waitForEnter();
                                }
                                break;
                            case 5:
                                if (user->canBorrow()) {
                                    handleReserveBook(library, userID);
                                    library.saveState();
                                    waitForEnter();
                                }
                                break;
                            case 6:
                                if (user->canBorrow()) {
                                    handleCancelReservation(library, userID);
                                    library.saveState();
                                    waitForEnter();
                                }
                                break;
                            case 7:
                                if (user->canBorrow()) {
                                    handleViewReservations(library, userID);
                                    waitForEnter();
                                }
                                break;
                            case 8:
                                if (user->canBorrow()) {
                                    handleViewBorrowedBooks(library, userID);
                                    waitForEnter();
                                }
                                break;
                            case 9:
                                if (user->canBorrow()) {
                                    handleViewFines(library, userID);
                                    waitForEnter();
                                }
                                break;
                            case 10:
                                if (user->canBorrow()) {
                                    handlePayFine(library, userID);
                                    library.saveState();
                                    waitForEnter();
                                }
                                break;
                            case 11:
                                if (user->canManageBooks()) {
                                    handleAddBook(library);
                                    library.saveState();
                                    waitForEnter();
                                }
                                break;
                            case 12:
                                if (user->canManageBooks()) {
                                    handleRemoveBook(library);
                                    library.saveState();
                                    waitForEnter();
                                }
                                break;
                            case 13:
                                if (user->canManageUsers()) {
                                    handleAddUser(library);
                                    library.saveState();
                                    waitForEnter();
                                }
                                break;
                            case 14:
                                if (user->canManageUsers()) {
                                    handleRemoveUser(library);
                                    library.saveState();
                                    waitForEnter();
                                }
                                break;
                            case 15:
                                if (user->canManageUsers()) {
                                    handleCheckUser(library);
                                    waitForEnter();
                                }
                                break;
                            case 16:
                                if (user->canManageUsers()) {
                                    handleViewAllBorrowedBooks(library);
                                    waitForEnter();
                                }
                                break;
                            default: 
                                cout << "Invalid choice!\n";
                                waitForEnter();
                                break;
                        }
                    }
                }
            } else {
                cout << "Invalid credentials!\n";
                waitForEnter();
            }
        } else {
            cout << "Invalid choice!\n";
            waitForEnter();
        }
    }

    return 0;
} 