#include <iostream>
#include <Windows.h>
#include <string>
#include <fstream>
using namespace std;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


// file processing

class FileProcessResult abstract
{
public:
    virtual void get_message() = 0;
};

template<typename T>
class FileInterface abstract
{
public:
    virtual FileProcessResult* open(ios_base::openmode mode) = 0;
    virtual FileProcessResult* close() = 0;
    virtual FileProcessResult* write(T data, ios_base::openmode mode) = 0;
    virtual FileProcessResult* read(T& data, ios_base::openmode mode) = 0;
};

class FileOpenSuccess : public FileProcessResult
{
public:
    FileOpenSuccess() = default;
    void get_message() override
    {
        cout << "File opened successfully" << endl;
    }
};

class FileOpenError : public FileProcessResult
{
public:
    FileOpenError() = default;
    void get_message() override
    {
        cout << "Error opening file" << endl;
    }
};

class FileCloseSuccess : public FileProcessResult
{
public:
    FileCloseSuccess() = default;
    void get_message() override
    {
        cout << "File closed successfully" << endl;
    }
};

class FileWriteSuccess : public FileProcessResult
{
public:
    FileWriteSuccess() = default;
    void get_message() override
    {
        cout << "Data written successfully" << endl;
    }
};

class FileWriteError : public FileProcessResult
{
    exception e;
public:
    FileWriteError() = default;
    FileWriteError(exception e)
    {
        this->e = e;
    }
    void get_message() override
    {
        cout << "Error writing data" << endl;
    }
};

class FileReadSuccess : public FileProcessResult
{
public:
    FileReadSuccess() = default;
    void get_message() override
    {
        cout << "Data read successfully" << endl;
    }
};

class FileReadError : public FileProcessResult
{
    exception e;
public:
    FileReadError() = default;
    FileReadError(exception e)
    {
        this->e = e;
    }
    void get_message() override
    {
        cout << "Error writing data" << endl;
    }
};


// file service

template<typename T>
class FileService : public FileInterface<T>
{
    string filename;
    fstream file;
public:
    FileService(string filename)
    {
        this->filename = filename;
    }
    FileProcessResult* open(ios_base::openmode mode) override
    {
        FileProcessResult* result = nullptr;
        file.open(filename, mode);
        if (file.is_open())
        {
            result = new FileOpenSuccess();
        }
        else
        {
            result = new FileOpenError();
        }
        return result;
    }
    FileProcessResult* close() override
    {
        file.close();
        return new FileCloseSuccess();
    }
    FileProcessResult* write(T data, ios_base::openmode mode) override
    {
        try
        {
            FileProcessResult* result = this->open(mode);
            result->get_message();
            if (typeid(*result) == typeid(FileOpenSuccess))
            {
                if (mode == ios::app || mode == ios::out || mode == (ios::out | ios::binary) || mode == (ios::app | ios::binary))
                {
                    file.write((char*)&data, sizeof(T));
                }
                else
                {
                    return new FileWriteError();
                }
                result = this->close();
                result->get_message();
                return new FileWriteSuccess();
            }
            else if (typeid(*result) == typeid(FileOpenError))
            {
                return new FileWriteError();
            }
        }
        catch (exception e)
        {
            return new FileWriteError(e);
        }
    }

    FileProcessResult* read(T& data, ios_base::openmode mode) override
    {
        try
        {
            FileProcessResult* result = this->open(mode);
            result->get_message();
            if (typeid(*result) == typeid(FileOpenSuccess))
            {
                if (mode == ios::in || mode == (ios::in | ios::binary))
                {
                    file.read((char*)&data, sizeof(T));
                }
                else
                {
                    return new FileWriteError();
                }
                result = this->close();
                result->get_message();
                return new FileReadSuccess();
            }
            else if (typeid(*result) == typeid(FileOpenError))
            {
                return new FileReadError();
            }
        }
        catch (exception e)
        {
            return new FileReadError(e);
        }
    }
};

// main classes
class User {

private:
    string name;
    string surname;
    string phone_number;

public:
    User() = default;
    User(string name, string surname, string phone_number)
        : name(name), surname(surname), phone_number(phone_number) {
    }


    void printInfo() const  {
        cout << "Guest: " << getName() << " " << getSurname()
            << ", Phone: " << getPhoneNumber() << endl;
    }

    string getName() const { return name; }
    string getSurname() const { return surname; }
    string getPhoneNumber() const { return phone_number; }

    void setName(const string& name) { this->name = name; }
    void setSurname(const string& surname) { this->surname = surname; }
    void setPhoneNumber(const string& phone_number) { this->phone_number = phone_number; }

    ~User() = default;
};


// main func

int main() {
    int choice;

    User user("Apple", "Doe", "+380230123");

    FileService<User> file("data.bin");
    
    FileProcessResult* result = nullptr;

    User new_user;
    while (true) {
        cout << "\n----------MENU------------\n";
        cout << "1.Enter a new user data" << endl;
        cout << "2.Write new user data to file" << endl; // Write - стирає файл перед тим як в нього написати
        cout << "3.Append new user data to file" << endl; // Append - додає до файлу нову інформацію не стираючи попередньої
        cout << "4.Read new user data from file" << endl;
        cout << "5.Exit" << endl;
        cout << "----------MENU------------\n";

        cin >> choice;

        cin.ignore();

        switch (choice) {
        case 1: {
            string name, surname, number;
            cout << "Enter a name: " << endl;
            getline(cin, name);
            cout << "Enter an surname: " << endl;
            getline(cin, surname);
            cout << "Enter a number: " << endl;
            getline(cin, number);
            User user(name, surname, number);
            break;
        }
        case 2:
            try {
                result = file.write(user, ios::out | ios::binary);
                result->get_message();
            }
            catch (FileProcessResult* file_error) {
                file_error->get_message();
            }
            break;
        case 3:
            try {
                result = file.write(user, ios::app | ios::binary);
                result->get_message();
            }
            catch (FileProcessResult* file_error) {
                file_error->get_message();
            }
            break;
        case 4:
            try {
                User new_user;
                result = file.read(new_user, ios::in | ios::binary);
                result->get_message();
                new_user.printInfo();
            }
            catch (FileProcessResult* file_error) {
                file_error->get_message();
            }
            break;
        case 5:
            exit(0);
        }
    }
    return 0;
}