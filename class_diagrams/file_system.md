# File System - Class Diagram

## System Design

```mermaid
classDiagram
direction TB

class FileSystemEntry {
    <<abstract>>
    -string name
    -string path
    -DateTime createdAt
    -DateTime modifiedAt
    +string getName()
    +string getPath()
    +bool isDirectory()
}

class File {
    -string content
    -string extension
    -int size
    +read()
    +write()
    +getSize()
}

class Directory {
    -Map~string, FileSystemEntry~ entries
    +addEntry()
    +removeEntry()
    +getEntry()
    +list()
}

class FileSystem {
    -Directory root
    +createFile()
    +createDirectory()
    +delete()
    +move()
    +search()
}

class Permission {
    -bool read
    -bool write
    -bool execute
    +checkPermission()
}

class User {
    -string username
    -List~Permission~ permissions
}

class FileSystemOperator {
    -FileSystem fs
    -User currentUser
    +performOperation()
}

FileSystemEntry <|-- File
FileSystemEntry <|-- Directory
Directory --> FileSystemEntry
FileSystem --> Directory
FileSystemEntry --> Permission
User --> Permission
FileSystemOperator --> FileSystem
FileSystemOperator --> User
```

## Design Patterns Used
1. Composite Pattern: For file/directory structure
2. Factory Pattern: For creating files and directories
3. Command Pattern: For file operations
4. Strategy Pattern: For different file operations
5. Observer: For file system changes

## Implementation Steps
1. Create base FileSystemEntry class
2. Implement File and Directory classes
3. Build main FileSystem class
4. Add permission system
5. Implement user management
6. Create file operations
7. Add path resolution
8. Implement search functionality
9. Add file system operators
10. Implement error handling
