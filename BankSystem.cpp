#include <iostream>
#include <string>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <vector>
#include <windows.h>
using namespace std;

string const BankClientsFile = "BankClients_2025.txt";
string const BankUsersFile = "BankUsers_2025.txt";

enum enMainMenuOptions
{
    RegisterNewClient = 1, ShowClients = 2,
    UpdateClientInfo = 3, DeleteClient = 4,
    FindClientInfo = 5, GoToTransactionsMenu = 6, GoToManageUsersMenu = 7, Logout = 8
};

enum enTransactionsMenuOptions
{
    DepositMoney = 1, WithdrawMoney = 2, ShowTotalBalances = 3, BackToMainMenu = 4
};

enum enManageUsersMenuOptions
{
    RegisterNewUser = 1, ShowUsers = 2,
    UpdateUserInfo = 3, DeleteUser = 4,
    FindUserInfo = 5, ReturnToMainMenu = 6
};

enum enMainMenuPermissions
{
    pHaveAllPermissions = -1, pAddNewClient = 1, pShowClientsTable = 2, pUpdateClient = 4,
    pDeleteClient = 8, pFindClient = 16, pWorkOnTransactionsMenu = 32, pWorkOnManageUsersMenu = 64
};

struct stBankUserInfo
{
    string Username;
    string Password;
    short Permissions = 0; // Full Access = -1
    bool MarkForDelete = false; // default value;
};

stBankUserInfo AuthenticatedUser;

void BankSystemLogo()
{
    cout << R"(
      ____             _    
     |  _ \           | |   
     | |_) | __ _ _ __| | __
     |  _ < / _` | '__| |/ /
     | |_) | (_| | |  |   < 
     |____/ \__,_|_|  |_|\_\
)" << endl << endl;
}

bool IsNumberInRange(short Number, short From, short To)
{
    return (Number >= From && Number <= To);
}

void ClearScreen()
{
    system("cls");
}

void SetConsoleTextColor(short ColorCode)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, ColorCode);
}

void PrintSuccessMessage(string SuccessfulMessage)
{
    SetConsoleTextColor(10);
    cout << "\n\t" << SuccessfulMessage;
    SetConsoleTextColor(7);
}

void PrintFailedMessage(string FailedMessage)
{
    SetConsoleTextColor(12);
    cout << "\n\t" << FailedMessage;
    SetConsoleTextColor(7);
}

bool CheckPermissionAccess(enMainMenuPermissions Permission)
{
    if (AuthenticatedUser.Permissions == enMainMenuPermissions::pHaveAllPermissions)
        return true;
    if ((Permission & AuthenticatedUser.Permissions) == Permission)
        return true;
    else
        return false;
}

void GoBackToMenu(string Message)
{
    cout << "\n\n" << Message;
    system("pause>0");
}

void ShowAccessDeniedScreen()
{
    ClearScreen();
    cout << "==============================================\n"
        << "\tAccess Denied,\n"
        << "\tYou don't have permission to do this,\n"
        << "\tPlease contact your admin."
        << "\n==============================================\n";
}

char AskToPerformOperation(string Message)
{
    char SurePerform;
    cout << "\n" << Message << ": Y/N? ";
    cin >> SurePerform;
    return SurePerform;
}

// Part1: Start Implementing the Main Menu with CRUD Operations:
// 1-Create: Add New Client Algorithm
struct stBankClientInfo
{
    string AccountNumber;
    string PinCode;
    string FullName;
    string PhoneNumber;
    double AccountBalance = 0.0;
    bool MarkForDelete = false; // default value;
};

void ShowAddClientScreen()
{
    ClearScreen();
    cout << "\t=======================================\n";
    cout << "\t|       BANK CLIENT REGISTRATION      |\n";
    cout << "\t=======================================\n\n";
}

bool IsClientExists(vector <stBankClientInfo>& vClientsRecords, string AccountNumber)
{
    for (stBankClientInfo& ClientRecord : vClientsRecords)
    {
        if (ClientRecord.AccountNumber == AccountNumber)
        {
            return true;
        }
    }
    return false;
}

stBankClientInfo ReadNewClientRecord(vector <stBankClientInfo>& vClientsRecords)
{
    stBankClientInfo NewClient;
    string AccountNumber;
    bool accountExists;

    do {
        cout << "\t+--------------------------------------+\n";
        cout << "\t|  Enter Account Number  : ";
        getline(cin >> ws, AccountNumber);
        cout << "\t+--------------------------------------+\n";

        accountExists = IsClientExists(vClientsRecords, AccountNumber);
        if (accountExists)
        {
            PrintFailedMessage("Account [" + AccountNumber + "] Already Exists!");
        }
    } while (accountExists);

    NewClient.AccountNumber = AccountNumber;

    cout << "\t|  Enter Pin Code        : ";
    cin >> NewClient.PinCode;
    cout << "\t+--------------------------------------+\n";

    cout << "\t|  Enter Full Name       : ";
    cin.ignore();
    getline(cin, NewClient.FullName);
    cout << "\t+--------------------------------------+\n";

    cout << "\t|  Enter Phone Number    : ";
    cin >> NewClient.PhoneNumber;
    cout << "\t+--------------------------------------+\n";

    cout << "\t|  Enter Balance ($)     : ";
    cin >> NewClient.AccountBalance;
    cout << "\t+--------------------------------------+\n";

    PrintSuccessMessage("Client Registered Successfully!");

    return NewClient;
}

string ConvertClientRecordToDataLine(stBankClientInfo NewClientRecord, string delimeter = "#//#")
{
    string NewDataLine;
    NewDataLine += NewClientRecord.AccountNumber + delimeter;
    NewDataLine += NewClientRecord.PinCode + delimeter;
    NewDataLine += NewClientRecord.FullName + delimeter;
    NewDataLine += NewClientRecord.PhoneNumber + delimeter;
    NewDataLine += to_string(NewClientRecord.AccountBalance);
    return NewDataLine;
}

void InsertNewDataLineToFile(string FileName, string NewDataLine)
{
    fstream File;
    File.open(FileName, ios::out | ios::app);

    if (File.is_open())
    {
        File << NewDataLine << endl;
        File.close();
    }
}

void AddNewClient(vector <stBankClientInfo>& vClientsRecords)
{
    stBankClientInfo NewClientRecord;
    NewClientRecord = ReadNewClientRecord(vClientsRecords);
    vClientsRecords.push_back(NewClientRecord);
    InsertNewDataLineToFile(BankClientsFile, ConvertClientRecordToDataLine(NewClientRecord));
}

void AddNewClients(vector <stBankClientInfo>& vClientsRecords)
{
    if (CheckPermissionAccess(enMainMenuPermissions::pAddNewClient) == false)
    {
        ShowAccessDeniedScreen();
        GoBackToMenu("Press any key to go back to Main Menu...");
        return;
    }
    do
    {
        ShowAddClientScreen();
        AddNewClient(vClientsRecords);
    } while (toupper(AskToPerformOperation("Do you want to add another client")) == 'Y');
    GoBackToMenu("Press any key to go back to Main Menu...");
}

// 2-Read: Show Clients Algorithm
void ShowClientsScreen()
{
    ClearScreen();
    cout << "\t=======================================\n";
    cout << "\t|         CLIENTS INFO SCREEN         |\n";
    cout << "\t=======================================\n\n";
}

vector<string> SplitString(string Text, string delimiterType)
{
    vector <string> vSplitTextWords{};
    string FullWord = " ";
    short delimiterIndex = 0;

    while ((delimiterIndex = Text.find(delimiterType)) != Text.npos)
    {
        if (Text != " ")
        {
            FullWord = Text.substr(0, delimiterIndex);
            vSplitTextWords.push_back(FullWord);
        }
        Text.erase(0, delimiterIndex + delimiterType.length()); // delete from 0 to number of delimiter characters
    }

    if (Text != " ")
    {
        vSplitTextWords.push_back(Text); // push last word
    }
    return vSplitTextWords;
}

stBankClientInfo ConvertClientDataLineToRecord(string DataLine)
{
    vector <string> vRecordInfo = SplitString(DataLine, "#//#");

    stBankClientInfo ClientRecord;
    ClientRecord.AccountNumber = vRecordInfo[0];
    ClientRecord.PinCode = vRecordInfo[1];
    ClientRecord.FullName = vRecordInfo[2];
    ClientRecord.PhoneNumber = vRecordInfo[3];
    ClientRecord.AccountBalance = stod(vRecordInfo[4]);
    return ClientRecord;
}

vector <stBankClientInfo> LoadClientsDataFromFileToVector(string FileName)
{
    string FileDataLine;
    stBankClientInfo ClientRecord;
    vector <stBankClientInfo> vClientsRecords{};

    fstream File;
    File.open(FileName, ios::in);

    if (File.is_open())
    {
        while (getline(File, FileDataLine)) //  || while (File << FileDataLine)
        {
            ClientRecord = ConvertClientDataLineToRecord(FileDataLine);
            vClientsRecords.push_back(ClientRecord);
        }
        File.close();
    }

    return vClientsRecords;
}

void ShowClientsTableColumns(vector <stBankClientInfo>& vClientsRecords)
{
    if (vClientsRecords.size() == 0)
    {
        cout << "\t\t\t\tNo Clients Available In the System!";
    }
    else
    {
        cout << endl << "\t\t\t\tClient List(" << vClientsRecords.size() << ") Client(s).";
    }
    cout << "\n_____________________________________________________________________________________________\n";
    cout << left << "| " << setw(15) << "Account Number" << "| " << setw(10) << "Pin Code"
        << "| " << setw(20) << "Client Name" << "| " << setw(15) << "Phone Number"
        << "| " << setw(10) << "Account Balance" << " |";
    cout << "\n_____________________________________________________________________________________________\n";
}

void ShowClientsTableCells(stBankClientInfo& ClientRecord)
{
    cout << "| " << left << setw(15) << ClientRecord.AccountNumber
        << "| " << setw(10) << ClientRecord.PinCode
        << "| " << setw(20) << ClientRecord.FullName
        << "| " << setw(15) << ClientRecord.PhoneNumber
        << "| " << setw(10) << ClientRecord.AccountBalance << " |\n";
}

void ShowClientsTable(vector <stBankClientInfo>& vClientsRecords)
{
    if (CheckPermissionAccess(enMainMenuPermissions::pShowClientsTable) == false)
    {
        ShowAccessDeniedScreen();
        GoBackToMenu("Press any key to go back to Main Menu...");
        return;
    }
    ShowClientsScreen();
    ShowClientsTableColumns(vClientsRecords);
    for (stBankClientInfo& Record : vClientsRecords)
    {
        ShowClientsTableCells(Record);
    }
    cout << "_____________________________________________________________________________________________\n";
    GoBackToMenu("Press any key to go back to Main Menu...");
}

// 3-Update: Update Client's Information Algorithm
void ShowUpdateClientScreen()
{
    ClearScreen();
    cout << "\t========================================\n";
    cout << "\t|       UPDATE CLIENT INFO SCREEN      |\n";
    cout << "\t========================================\n\n";
}

string ReadAccountNumber()
{
    string AccountNumber;
    cout << "Enter Account Number? ";
    cin >> AccountNumber;
    cout << endl;
    return AccountNumber;
}

bool SearchAboutClient(vector <stBankClientInfo>& vClientsRecords, string AccountNumber, stBankClientInfo& CorrectClient)
{
    for (stBankClientInfo& Record : vClientsRecords)
    {
        if (AccountNumber == Record.AccountNumber)
        {
            CorrectClient = Record;
            return true;
        }
    }
    return false;
}

void PrintClientRecord(stBankClientInfo& ClientRecord)
{
    cout << "\t.------------------------------------------.\n"
        << "\t|          CLIENT INFORMATION              |\n"
        << "\t|------------------------------------------|\n"
        << "\t| . Account Number : " << setw(20) << left << ClientRecord.AccountNumber << "  |\n"
        << "\t| . Pin Code       : " << setw(20) << left << ClientRecord.PinCode << "  |\n"
        << "\t| . Full Name      : " << setw(20) << left << ClientRecord.FullName << "  |\n"
        << "\t| . Phone Number   : " << setw(20) << left << ClientRecord.PhoneNumber << "  |\n"
        << "\t| . Account Balance: " << setw(20) << left << ClientRecord.AccountBalance << "  |\n"
        << "\t.------------------------------------------.\n";
}

stBankClientInfo UpdateClientRecord(stBankClientInfo& CorrectClient)
{
    cout << "\t+--------------------------------------+\n";
    cout << "\t|  Client's Account Number : " << CorrectClient.AccountNumber << "          |\n";
    cout << "\t+--------------------------------------+\n";

    cout << "\t|  Enter New Pin Code      : ";
    cin >> CorrectClient.PinCode;
    cout << "\t+--------------------------------------+\n";

    cout << "\t|  Enter New Full Name     : ";
    cin.ignore();
    getline(cin, CorrectClient.FullName);
    cout << "\t+--------------------------------------+\n";

    cout << "\t|  Enter New Phone Number  : ";
    cin >> CorrectClient.PhoneNumber;
    cout << "\t+--------------------------------------+\n";

    cout << "\t|  Enter New Balance ($)   : ";
    cin >> CorrectClient.AccountBalance;
    cout << "\t+--------------------------------------+\n";

    return CorrectClient;
}

void UpdateClientIndex(vector <stBankClientInfo>& vClientsRecords, string AccountNumber, stBankClientInfo& UpdatedClientRecord)
{
    for (stBankClientInfo& OldClientRecord : vClientsRecords)
    {
        if (OldClientRecord.AccountNumber == AccountNumber)
        {
            OldClientRecord = UpdatedClientRecord;
            break;
        }
    }
}

void LoadClientsDataFromVectorToFileAfterUpdate(vector <stBankClientInfo>& vClientsRecords, string FileName)
{
    string FileDataLine;

    fstream File;
    File.open(FileName, ios::out);

    if (File.is_open())
    {
        for (stBankClientInfo& ClientRecord : vClientsRecords)
        {
            FileDataLine = ConvertClientRecordToDataLine(ClientRecord);
            File << FileDataLine << endl;
        }
        File.close();
    }
}

bool UpdateClientByAccountNumber(vector <stBankClientInfo>& vClientsRecords)
{
    if (CheckPermissionAccess(enMainMenuPermissions::pUpdateClient) == false)
    {
        ShowAccessDeniedScreen();
        return false;
    }
    ShowUpdateClientScreen();

    string AccountNumber;
    AccountNumber = ReadAccountNumber();
    stBankClientInfo CorrectClient;

    if (SearchAboutClient(vClientsRecords, AccountNumber, CorrectClient))
    {
        PrintClientRecord(CorrectClient);
        if (toupper(AskToPerformOperation("Are you sure you want to update this client")) == 'Y')
        {
            UpdateClientRecord(CorrectClient);
            UpdateClientIndex(vClientsRecords, AccountNumber, CorrectClient);
            LoadClientsDataFromVectorToFileAfterUpdate(vClientsRecords, BankClientsFile);
            PrintSuccessMessage("Client Updated Successfully!");
            return true;
        }
        else
        {
            PrintFailedMessage("Failed to Update Client!");
            return false;
        }
    }

    PrintFailedMessage("Account [" + AccountNumber + "] not found!");
    {
        return false;
    }
}

// 4-Delete: Delete Client Algorithm
void ShowDeleteClientScreen()
{
    ClearScreen();
    cout << "\t========================================\n";
    cout << "\t|         DELETE CLIENT SCREEN         |\n";
    cout << "\t========================================\n\n";
}

stBankClientInfo MarkClientRecordForDelete(stBankClientInfo& CorrectClient)
{
    CorrectClient.MarkForDelete = true;
    return CorrectClient;
}

void MarkClientIndexForDelete(vector <stBankClientInfo>& vClientsRecords, string AccountNumber, stBankClientInfo& MarkedRecordForDeletion)
{
    for (stBankClientInfo& OldClientRecord : vClientsRecords)
    {
        if (OldClientRecord.AccountNumber == AccountNumber)
        {
            OldClientRecord = MarkedRecordForDeletion;
            break;
        }
    }
}

void LoadClientsDataFromVectorToFileAfterDelete(vector <stBankClientInfo>& vClientsRecords, string FileName)
{
    string FileDataLine;

    fstream File;
    File.open(FileName, ios::out);

    if (File.is_open())
    {
        for (stBankClientInfo& ClientRecord : vClientsRecords)
        {
            if (ClientRecord.MarkForDelete != true)
            {
                FileDataLine = ConvertClientRecordToDataLine(ClientRecord);
                File << FileDataLine << endl;
            }
        }
        File.close();
    }
}

vector <stBankClientInfo> RefreashVector(string FileName, vector <stBankClientInfo>& vClientsRecords)
{
    stBankClientInfo Record;
    string FileDataLine;

    fstream File;
    File.open(FileName, ios::in);
    vClientsRecords.clear();

    if (File.is_open())
    {
        while (getline(File, FileDataLine))
        {
            Record = ConvertClientDataLineToRecord(FileDataLine);
            if (Record.MarkForDelete != true)
            {
                vClientsRecords.push_back(Record);
            }
        }
        File.close();
    }
    return vClientsRecords;
}

bool DeleteClientByAccountNumber(vector <stBankClientInfo>& vClientsRecords)
{
    if (CheckPermissionAccess(enMainMenuPermissions::pDeleteClient) == false)
    {
        ShowAccessDeniedScreen();
        return false;
    }
    ShowDeleteClientScreen();

    string AccountNumber;
    AccountNumber = ReadAccountNumber();
    stBankClientInfo CorrectClient;

    if (SearchAboutClient(vClientsRecords, AccountNumber, CorrectClient))
    {
        PrintClientRecord(CorrectClient);
        if (toupper(AskToPerformOperation("Are you sure you want to delete this client")) == 'Y')
        {
            MarkClientRecordForDelete(CorrectClient);
            MarkClientIndexForDelete(vClientsRecords, AccountNumber, CorrectClient);
            LoadClientsDataFromVectorToFileAfterDelete(vClientsRecords, BankClientsFile);
            vClientsRecords = RefreashVector(BankClientsFile, vClientsRecords); // Refresh Clients
            PrintSuccessMessage("Client Deleted Successfully!");
            return true;
        }
        else
        {
            PrintFailedMessage("Failed to Delete Client!");
            return false;
        }
    }

    PrintFailedMessage("Account [" + AccountNumber + "] not found!");
    {
        return false;
    }
}

// 5-Search about Client Algorithm
void ShowFindClientScreen()
{
    ClearScreen();
    cout << "\t=========================================\n";
    cout << "\t|       SEARCH ABOUT CLIENT SCREEN      |\n";
    cout << "\t=========================================\n\n";
}

void FindClientByAccountNumber(vector <stBankClientInfo>& vClientsRecords)
{
    if (CheckPermissionAccess(enMainMenuPermissions::pFindClient) == false)
    {
        ShowAccessDeniedScreen();
        GoBackToMenu("Press any key to go back to Main Menu...");
        return;
    }
    ShowFindClientScreen();
    string AccountNumber = ReadAccountNumber();
    stBankClientInfo CorrectClient;

    if (SearchAboutClient(vClientsRecords, AccountNumber, CorrectClient))
    {
        PrintClientRecord(CorrectClient);
    }
    else
    {
        PrintFailedMessage("Account [" + AccountNumber + "] not found!");
    }
    GoBackToMenu("Press any key to go back to Main Menu...");
}


// Part2: Start Implementing the Transactions Menu:
// 1-Deposit Operation
void ShowDepositScreen()
{
    ClearScreen();
    cout << "\t=======================================\n";
    cout << "\t|         DEPOSIT MONY SCREEN         |\n";
    cout << "\t=======================================\n\n";
}

double ReadDepositAmount()
{
    double Deposit = 0.0;
    cout << "\nPlease enter Amount of Deposit: ";
    cin >> Deposit;
    return Deposit;
}

stBankClientInfo UpdateAccountBalanceRecordByDeposit(stBankClientInfo& CorrectClient, double DepositAmount)
{
    CorrectClient.AccountBalance += DepositAmount;
    return CorrectClient;
}

void UpdateAccountBalanceIndexByDeposit(vector <stBankClientInfo>& vClientsRecords, string AccountNumber, stBankClientInfo& UpdatedAccountBalance)
{
    for (stBankClientInfo& OldAccountBalance : vClientsRecords)
    {
        if (OldAccountBalance.AccountNumber == AccountNumber)
        {
            OldAccountBalance.AccountBalance = UpdatedAccountBalance.AccountBalance;
            break;
        }
    }
}

bool DepositBalanceToClientByAccountNumber(vector <stBankClientInfo>& vClientsRecords)
{
    string AccountNumber;
    stBankClientInfo CorrectClient;
    double DepositAmount = 0.0;

    ShowDepositScreen();
    AccountNumber = ReadAccountNumber();
    while (!SearchAboutClient(vClientsRecords, AccountNumber, CorrectClient))
    {
        PrintFailedMessage("Account [" + AccountNumber + "] dose not exist!");
        AccountNumber = ReadAccountNumber();
    }

    PrintClientRecord(CorrectClient);
    DepositAmount = ReadDepositAmount();

    if (toupper(AskToPerformOperation("Are you sure you want to perform this transaction")) == 'Y')
    {
        UpdateAccountBalanceRecordByDeposit(CorrectClient, DepositAmount);
        UpdateAccountBalanceIndexByDeposit(vClientsRecords, AccountNumber, CorrectClient);
        LoadClientsDataFromVectorToFileAfterUpdate(vClientsRecords, BankClientsFile);
        PrintSuccessMessage("Deposit Successfully!");
        return true;
    }
    else
    {
        PrintFailedMessage("Failed to Deposit!");
        return false;
    }
}

// 2-Withdraw Operation
void ShowWithdrawScreen()
{
    ClearScreen();
    cout << "\t=======================================\n";
    cout << "\t|        WITHDRAW MONY SCREEN         |\n";
    cout << "\t=======================================\n\n";
}

double ReadWithdrawAmount(string Message)
{
    double Withdraw = 0.0;
    cout << Message;
    cin >> Withdraw;
    return Withdraw;
}

bool IsWithdrawAmountExceedingBalance(double WithdrawAmount, stBankClientInfo& CorrectClient)
{
    return (WithdrawAmount > CorrectClient.AccountBalance);
}

stBankClientInfo UpdateAccountBalanceRecordByWithdraw(stBankClientInfo& CorrectClient, double WithdrawAmount)
{
    UpdateAccountBalanceRecordByDeposit(CorrectClient, WithdrawAmount * -1);
    return CorrectClient;
}

void UpdateAccountBalanceIndexByWithdraw(vector <stBankClientInfo>& vClientsRecords, string AccountNumber, stBankClientInfo& UpdatedAccountBalance)
{
    for (stBankClientInfo& OldAccountBalance : vClientsRecords)
    {
        if (OldAccountBalance.AccountNumber == AccountNumber)
        {
            OldAccountBalance.AccountBalance = UpdatedAccountBalance.AccountBalance;
            break;
        }
    }
}

bool WithdrawBalanceFromClientByAccountNumber(vector <stBankClientInfo>& vClientsRecords)
{
    stBankClientInfo CorrectClient;
    string AccountNumber;
    double WithdrawAmount = 0.0;

    ShowWithdrawScreen();
    AccountNumber = ReadAccountNumber();
    while (!SearchAboutClient(vClientsRecords, AccountNumber, CorrectClient))
    {
        PrintFailedMessage("Account [" + AccountNumber + "] dose not exist!");
        AccountNumber = ReadAccountNumber();
    }
    PrintClientRecord(CorrectClient);

    WithdrawAmount = ReadWithdrawAmount("\nPlease enter Amount of Withdraw: ");
    while (IsWithdrawAmountExceedingBalance(WithdrawAmount, CorrectClient))
    {
        cout << "\nOops, Withdraw Amount Exceeds the balance, you can withdraw up to [" << CorrectClient.AccountBalance << "]" << endl;
        WithdrawAmount = ReadWithdrawAmount("\nPlease enter another Withdraw Amount: ");
    }

    if (toupper(AskToPerformOperation("Are you sure you want to perform this transaction")) == 'Y')
    {
        UpdateAccountBalanceRecordByWithdraw(CorrectClient, WithdrawAmount);
        UpdateAccountBalanceIndexByWithdraw(vClientsRecords, AccountNumber, CorrectClient);
        LoadClientsDataFromVectorToFileAfterUpdate(vClientsRecords, BankClientsFile);
        PrintSuccessMessage("Withdraw Successfully!");
        return true;
    }
    else
    {
        PrintFailedMessage("Failed to Withdraw!");
        return false;
    }
}

// 3-Read: Show Total Balances Operation
void ShowTotalBalancesScreen()
{
    ClearScreen();
    cout << "\t=======================================\n";
    cout << "\t|       TOTAL BALANCES SCREEN         |\n";
    cout << "\t=======================================\n\n";
}

short CalculateNumberOfClients(vector <stBankClientInfo>& vClientsRecords)
{
    short ClientsNumber = 0;
    for (short i = 0; i < vClientsRecords.size(); i++)
    {
        ClientsNumber++;
    }
    return ClientsNumber;
}

void ShowTotalBalancesTableColumns(vector <stBankClientInfo>& vClientsRecords)
{
    short ClientsNumber = CalculateNumberOfClients(vClientsRecords);
    if (ClientsNumber == 0)
        cout << "\n\n\t\t\t\tNo Clients Available In the System!\n";
    else
        cout << "\n\n\t\t\t\tClient List (" << ClientsNumber << ") Client(s)\n";

    cout << "\t\t\t     _____________________________________________________\n";
    cout << "\t\t\t     | " << left << setw(15) << "Account Number"
        << "| " << setw(20) << "Client Name"
        << "| " << setw(10) << "Balance" << " |\n";
    cout << "\t\t\t     _____________________________________________________\n";
}

void ShowTotalBalancesTableCells(stBankClientInfo& ClientRecord)
{
    cout << "\t\t\t     | " << left << setw(15) << ClientRecord.AccountNumber
        << "| " << setw(20) << ClientRecord.FullName
        << "| " << setw(10) << ClientRecord.AccountBalance << " |\n";
}

int CalculateTotalBalances(vector <stBankClientInfo>& vClientsRecords)
{
    int TotalBalances = 0;
    for (stBankClientInfo& ClientRecord : vClientsRecords)
    {
        TotalBalances += ClientRecord.AccountBalance;
    }
    return TotalBalances;
}

void PrintTotalBalances(int TotalBalances)
{
    cout << "\t\t\t     |___________________________________________________|\n";
    cout << "\n\t\t\t\t" << "Total:  " << TotalBalances << "$";
}

void PrintTotalBalancesTable(vector <stBankClientInfo>& vClientsRecords)
{
    ShowTotalBalancesScreen();
    ShowTotalBalancesTableColumns(vClientsRecords);
    for (stBankClientInfo& Record : vClientsRecords)
    {
        ShowTotalBalancesTableCells(Record);
    }
    PrintTotalBalances(CalculateTotalBalances(vClientsRecords));
    GoBackToMenu("Press any key to go back to Transactions Menu...");
}

// Financial Transactions Menu Declaration:
void PlayBankProjectFromTransactionsMenu(vector<stBankClientInfo>& vClientsRecords);

// Manage Users Menu Declaration:
vector <stBankUserInfo> vUsersRecords;

void PlayBankProjectFromManageUsersMenu();

// Play the Main Menu
void Login();

void ShowMainMenu()
{
    ClearScreen();
    BankSystemLogo();
    cout << "\t|----------------------------------------|\n";
    cout << "\t|             BANK SYSTEM MENU           |\n";
    cout << "\t|----------------------------------------|\n";
    cout << "\t|        [1] Add New Client              |\n";
    cout << "\t|        [2] Show Clients List           |\n";
    cout << "\t|        [3] Update Client Information   |\n";
    cout << "\t|        [4] Delete Client               |\n";
    cout << "\t|        [5] Search for Client           |\n";
    cout << "\t|        [6] Transactions Menu           |\n";
    cout << "\t|        [7] Manage Users Menu           |\n";
    cout << "\t|        [8] Logout                      |\n";
    cout << "\t|----------------------------------------|\n\n";
    cout << "\tChoose what do you want from [1] to [8]: ";
}

enMainMenuOptions ReadMainMenuOption()
{
    short OperationNumber = 0;
    cin >> OperationNumber;
    while (!IsNumberInRange(OperationNumber, 1, 8))
    {
        PrintFailedMessage("Invalid Number! Enter number between [1] and [8]: ");
        cin >> OperationNumber;
    }
    return (enMainMenuOptions)OperationNumber;
}

void PerformMainMenuOption(enMainMenuOptions OperationType, vector<stBankClientInfo>& vClientsRecords)
{
    switch (OperationType)
    {
    case RegisterNewClient:
        AddNewClients(vClientsRecords);
        break;
    case ShowClients:
        ShowClientsTable(vClientsRecords);
        break;
    case UpdateClientInfo:
        UpdateClientByAccountNumber(vClientsRecords);
        GoBackToMenu("Press any key to go back to Main Menu...");
        break;
    case DeleteClient:
        DeleteClientByAccountNumber(vClientsRecords);
        GoBackToMenu("Press any key to go back to Main Menu...");
        break;
    case FindClientInfo:
        FindClientByAccountNumber(vClientsRecords);
        break;
    case GoToTransactionsMenu:
        PlayBankProjectFromTransactionsMenu(vClientsRecords);
        break;
    case GoToManageUsersMenu:
        PlayBankProjectFromManageUsersMenu();
        break;
    case Logout:
        Login();
        break;
    }
}

void PlayBankProjectFromMainMenu(vector<stBankClientInfo>& vClientsRecords)
{
    enMainMenuOptions OperationType;
    do
    {
        ShowMainMenu();
        OperationType = ReadMainMenuOption();
        PerformMainMenuOption(OperationType, vClientsRecords);
    } while (OperationType != enMainMenuOptions::Logout);
}


// Part3: Start Implementing the Manage Users Menu with CRUD Operations + Create Login System
// 1- Permissions Algorithm:
short GetPermissionsValue()
{
    short PermissionsValue = 0;

    if (toupper(AskToPerformOperation("Do you want to give this user all system's Permissions")) == 'Y')
    {
        return -1;
    }

    cout << "\nDo you want to give user access to:\n";

    if (toupper(AskToPerformOperation("Add New Client")) == 'Y')
    {
        PermissionsValue += enMainMenuPermissions::pAddNewClient;
    }
    if (toupper(AskToPerformOperation("Show Clients List")) == 'Y')
    {
        PermissionsValue += enMainMenuPermissions::pShowClientsTable;
    }
    if (toupper(AskToPerformOperation("Update Client Information")) == 'Y')
    {
        PermissionsValue += enMainMenuPermissions::pUpdateClient;
    }
    if (toupper(AskToPerformOperation("Delete Client")) == 'Y')
    {
        PermissionsValue += enMainMenuPermissions::pDeleteClient;
    }
    if (toupper(AskToPerformOperation("Search About Client")) == 'Y')
    {
        PermissionsValue += enMainMenuPermissions::pFindClient;
    }
    if (toupper(AskToPerformOperation("Work On Transactions Menu")) == 'Y')
    {
        PermissionsValue += enMainMenuPermissions::pWorkOnTransactionsMenu;
    }
    if (toupper(AskToPerformOperation("Work On Manage Users Menu")) == 'Y')
    {
        PermissionsValue += enMainMenuPermissions::pWorkOnManageUsersMenu;
    }

    return PermissionsValue;
}

// 2-Create: Add New User Algorithm
void ShowAddUserScreen()
{
    ClearScreen();
    cout << "\t=======================================\n";
    cout << "\t|       BANK USER REGISTRATION        |\n";
    cout << "\t=======================================\n\n";
}

bool IsUserExists(string UserName)
{
    for (stBankUserInfo& UserRecord : vUsersRecords)
    {
        if (UserRecord.Username == UserName)
        {
            return true;
        }
    }
    return false;
}

stBankUserInfo ReadNewUserRecord()
{
    stBankUserInfo NewUser;
    string UserName;
    cout << "\nAdding New User:";

    cout << "\nEnter Username: ";
    getline(cin >> ws, UserName);
    while (IsUserExists(UserName))
    {
        PrintFailedMessage("Username [" + UserName + "] Already Exists!");
        cout << "\nEnter Username: ";
        getline(cin >> ws, UserName);
    }
    NewUser.Username = UserName;

    cout << "\nEnter Password: ";
    cin >> NewUser.Password;

    NewUser.Permissions = GetPermissionsValue();

    return NewUser;
}

string ConvertUserRecordToDataLine(stBankUserInfo NewUser, string delimeter = "#//#")
{
    return NewUser.Username + delimeter + NewUser.Password + delimeter + to_string(NewUser.Permissions);
}

void AddNewUser()
{
    stBankUserInfo NewUserRecord;
    NewUserRecord = ReadNewUserRecord();
    vUsersRecords.push_back(NewUserRecord);
    InsertNewDataLineToFile(BankUsersFile, ConvertUserRecordToDataLine(NewUserRecord, "#//#"));
}

void AddNewUsers()
{
    ShowAddUserScreen();
    do
    {
        AddNewUser();
        PrintSuccessMessage("User Added Successfully!");
    } while (toupper(AskToPerformOperation("Do you want to add another user")) == 'Y');
    GoBackToMenu("Press any key to go back to Manage Users Menu...");
}

// 3-Read: Show Users Algorithm
void ShowUsersScreen()
{
    ClearScreen();
    cout << "\t=====================================\n";
    cout << "\t|         USERS INFO SCREEN         |\n";
    cout << "\t=====================================\n\n";
}

stBankUserInfo ConvertUserDataLineToRecord(string DataLine)
{
    vector <string> vRecordInfo = SplitString(DataLine, "#//#");
    stBankUserInfo UserRecord;
    UserRecord.Username = vRecordInfo[0];
    UserRecord.Password = vRecordInfo[1];
    UserRecord.Permissions = stoi(vRecordInfo[2]);
    return UserRecord;
}

vector <stBankUserInfo> LoadUsersDataFromFileToVector(string FileName)
{
    string FileDataLine;
    stBankUserInfo UserRecord;
    vector <stBankUserInfo> vUsersRecords{};

    fstream File;
    File.open(FileName, ios::in);

    if (File.is_open())
    {
        while (getline(File, FileDataLine))
        {
            UserRecord = ConvertUserDataLineToRecord(FileDataLine);
            vUsersRecords.push_back(UserRecord);
        }
        File.close();
    }

    return vUsersRecords;
}

void ShowUsersTableColumns()
{
    if (vUsersRecords.size() == 0)
    {
        cout << "\t\t\t\tNo Users Available In the System!";
    }
    else
    {
        cout << endl << "\t\t\t\tUser List(" << vUsersRecords.size() << ") User(s).";
    }
    cout << "\n_______________________________________________________________\n";
    cout << left << "| " << setw(15) << "Username"
        << "| " << setw(15) << "Password"
        << "| " << setw(15) << "Permissions" << " |\n";
    cout << "_______________________________________________________________\n";
}

void ShowUsersTableCells(stBankUserInfo& UserRecord)
{
    cout << "| " << left << setw(15) << UserRecord.Username
        << "| " << setw(15) << UserRecord.Password
        << "| " << setw(15) << UserRecord.Permissions << " |\n";
}

void ShowUsersTable()
{
    ShowUsersScreen();
    ShowUsersTableColumns();
    for (stBankUserInfo& UserRecord: vUsersRecords)
    {
        ShowUsersTableCells(UserRecord);
    }
    cout << "_______________________________________________________________\n";
    GoBackToMenu("Press any key to go back to Manage Users Menu...");
}

// 4-Update: Update User Information Algorithm
void ShowUpdateUserScreen()
{
    ClearScreen();
    cout << "\t========================================\n";
    cout << "\t|        UPDATE USER INFO SCREEN       |\n";
    cout << "\t========================================\n\n";
}

string ReadUsername()
{
    string Username;
    cout << "\nPlease enter username: ";
    cin >> Username;
    return Username;
}

bool SearchAboutUser(string Username, stBankUserInfo& FoundUserRecord)
{
    for (stBankUserInfo& Record : vUsersRecords)
    {
        if (Record.Username == Username)
        {
            FoundUserRecord = Record;
            return true;
        }
    }
    return false;
}

void PrintUserRecord(stBankUserInfo& FoundUserRecord)
{
    cout << "\t.-----------------------------.\n"
        << "\t|      USER INFORMATION       |\n"
        << "\t|-----------------------------|\n"
        << "\t| Username    : " << left << setw(14) << FoundUserRecord.Username << "|\n"
        << "\t| Password    : " << left << setw(14) << FoundUserRecord.Password << "|\n"
        << "\t| Permissions : " << left << setw(14) << FoundUserRecord.Permissions << "|\n"
        << "\t'-----------------------------'\n";

}

stBankUserInfo UpdateUserRecord(string Username)
{
    stBankUserInfo UpdatedUserRecord;
    UpdatedUserRecord.Username = Username;

    cout << "\t+--------------------------------------+\n";
    cout << "\t|  User's Username      : " << UpdatedUserRecord.Username << "\n";
    cout << "\t+--------------------------------------+\n";
    cout << "\t|  Enter New Password   : ";
    cin >> UpdatedUserRecord.Password;
    cout << "\t+--------------------------------------+\n";
    cout << "\t|  Enter New Permissions: ";
    UpdatedUserRecord.Permissions = GetPermissionsValue();
    cout << "\t+--------------------------------------+\n";

    return UpdatedUserRecord;
}

void UpdateUserIndex(stBankUserInfo& UpdatedUserRecord)
{
    for (stBankUserInfo& OldUserRecord : vUsersRecords)
    {
        if (OldUserRecord.Username == UpdatedUserRecord.Username)
        {
            OldUserRecord = UpdatedUserRecord;
            break;
        }
    }
}

void LoadUsersDateToFileAfterUpdate(string FileName)
{
    string FileDateLine;
    fstream File;

    File.open(FileName, ios::out);

    if (File.is_open())
    {
        for (stBankUserInfo& UserRecord : vUsersRecords)
        {
            FileDateLine = ConvertUserRecordToDataLine(UserRecord);
            File << FileDateLine << endl;
        }
        File.close();
    }
}

bool UpdateUserByUsername()
{
    ShowUpdateUserScreen();

    string Username;
    Username = ReadUsername();
    stBankUserInfo CorrectUserToUpdate;

    if (SearchAboutUser(Username, CorrectUserToUpdate))
    {
        PrintUserRecord(CorrectUserToUpdate);
        if (toupper(AskToPerformOperation("Are you sure you want to update this client")) == 'Y')
        {
            CorrectUserToUpdate = UpdateUserRecord(Username);
            UpdateUserIndex(CorrectUserToUpdate);
            LoadUsersDateToFileAfterUpdate(BankUsersFile);
            PrintSuccessMessage("User Updated Successfully!");
            return true;
        }
        else
        {
            PrintFailedMessage("Failed to update User!");
            return false;
        }
    }
    else
    {
        PrintFailedMessage("User with username is not found!");
        return false;
    }
}

// 5-Delete: Delete User Algorithm
void ShowDeleteUserScreen()
{
    ClearScreen();
    cout << "\t========================================\n";
    cout << "\t|          DELETE USER SCREEN          |\n";
    cout << "\t========================================\n\n";
}

stBankUserInfo MarkRecordForDelete()
{
    stBankUserInfo CorrectUserToDelete;
    CorrectUserToDelete.MarkForDelete = true;
    return CorrectUserToDelete;
}

void MarkUserIndexForDelete(string Username, stBankUserInfo MarkedUserRecordToDelete)
{
    for (stBankUserInfo& TargetedUserRecord : vUsersRecords)
    {
        if (TargetedUserRecord.Username == Username)
        {
            TargetedUserRecord = MarkedUserRecordToDelete;
            break;
        }
    }
}

void LoadUsersDateFromVectorToFileAfterDelete(string FileName)
{
    string FileDateLine;
    fstream File;
    File.open(FileName, ios::out);

    if (File.is_open())
    {
        for (stBankUserInfo& UserRecord : vUsersRecords)
        {
            if (UserRecord.MarkForDelete != true)
            {
                FileDateLine = ConvertUserRecordToDataLine(UserRecord);
                File << FileDateLine << endl;
            }
        }
        File.close();
    }
}

vector <stBankUserInfo> RefreshUsersVector(string FileName)
{
    vUsersRecords.clear();
    string FileDataLine;
    fstream File;
    stBankUserInfo Record;

    File.open(FileName, ios::in);
    if (File.is_open())
    {
        while (getline(File, FileDataLine))
        {
            Record = ConvertUserDataLineToRecord(FileDataLine);
            if (Record.MarkForDelete != true)
            {
                vUsersRecords.push_back(Record);
            }
        }
        File.close();
    }
    return vUsersRecords;
}

bool DeleteUserByUsername()
{
    ShowDeleteUserScreen();
    string Username;
    Username = ReadUsername();
    stBankUserInfo CorrectUserToDelete;

    if (SearchAboutUser(Username, CorrectUserToDelete))
    {
        PrintUserRecord(CorrectUserToDelete);
        if (toupper(AskToPerformOperation("Are you sure you want to delete this user")) == 'Y')
        {
            CorrectUserToDelete = MarkRecordForDelete();
            MarkUserIndexForDelete(Username, CorrectUserToDelete);
            LoadUsersDateFromVectorToFileAfterDelete(BankUsersFile);
            vUsersRecords = RefreshUsersVector(BankUsersFile);
            PrintSuccessMessage("User Deleted Successfully!");
            return true;
        }
        else
        {
            PrintFailedMessage("Failed to delete User!");
            return false;
        }

    }
    else
    {
        PrintFailedMessage("User with username is not found!");
        return false;
    }
}

// 6-Search about User Algorithm
void ShowFindUserScreen()
{
    ClearScreen();
    cout << "\t=========================================\n";
    cout << "\t|       SEARCH ABOUT USER SCREEN        |\n";
    cout << "\t=========================================\n\n";
}

void FindUserByUsername()
{
    ShowFindUserScreen();
    string Username;
    Username = ReadUsername();
    stBankUserInfo FoundUserRecord;

    if (SearchAboutUser(Username, FoundUserRecord))
    {
        PrintUserRecord(FoundUserRecord);
    }
    else
    {
        PrintFailedMessage("Username [" + Username + "] not found!");
    }
    GoBackToMenu("Press any key to go back to Manage Users Menu...");
}

// 7-Start Implementing Login System
void ShowLoginScreen()
{
    ClearScreen();
    cout << "\t========================================\n";
    cout << "\t|       HELLO, THIS LOGIN SCREEN       |\n";
    cout << "\t========================================\n\n";
}

string ReadPassword()
{
    string Password;
    cout << "\nEnter Password? ";
    cin >> Password;
    return Password;
}

bool IsLoginValid (string Username, string Password, stBankUserInfo& AuthenticatedUser)
{
    for (stBankUserInfo& UserRecord : vUsersRecords)
    {
        if (UserRecord.Username == Username && UserRecord.Password == Password)
        {
            AuthenticatedUser = UserRecord;
            return true;
        }
    }
    return false;
}

void Login()
{
    ShowLoginScreen();
    vUsersRecords = LoadUsersDataFromFileToVector(BankUsersFile);
    bool ValidLogin;
    do
    {
        string Username, Password;
        Username = ReadUsername();
        Password = ReadPassword();
        ValidLogin = IsLoginValid(Username, Password, AuthenticatedUser);
        if (ValidLogin)
        {
            break;
        }
        ShowLoginScreen();
        PrintFailedMessage("Invalid Username/Password!\n");
    } while (!ValidLogin);

    vector <stBankClientInfo> vClientsRecords = LoadClientsDataFromFileToVector(BankClientsFile);
    PlayBankProjectFromMainMenu(vClientsRecords);
}

int main()
{
    Login();
    return 0;
}

// Play Transactions Menu
void ShowTransactionsMenu()
{
    ClearScreen();
    BankSystemLogo();
    cout << "\t|--------------------------------------|\n";
    cout << "\t|       TRANSACTIONS MENU SCREEN       |\n";
    cout << "\t|--------------------------------------|\n";
    cout << "\t|       [1] Deposit                    |\n";
    cout << "\t|       [2] Withdraw                   |\n";
    cout << "\t|       [3] Total Balances             |\n";
    cout << "\t|       [4] Back To Main Menu          |\n";
    cout << "\t|--------------------------------------|\n\n";
    cout << "\tChoose what do you want from [1] to [4]: ";
}

enTransactionsMenuOptions ReadTransactionsMenuOption()
{
    short TransactionNumber = 0;
    cin >> TransactionNumber;
    while (!IsNumberInRange(TransactionNumber, 1, 4))
    {
        PrintFailedMessage("Invalid Number! Enter number between [1] to [4]: ");
        cin >> TransactionNumber;
    }
    return (enTransactionsMenuOptions)TransactionNumber;
}

bool PerformTransactionsMenuOption(enTransactionsMenuOptions TransactionType, vector <stBankClientInfo>& vClientsRecords)
{
    switch (TransactionType)
    {
    case DepositMoney:
        DepositBalanceToClientByAccountNumber(vClientsRecords);
        GoBackToMenu("Press any key to go back to Transactions Menu...");
        return false;
    case WithdrawMoney:
        WithdrawBalanceFromClientByAccountNumber(vClientsRecords);
        GoBackToMenu("Press any key to go back to Transactions Menu...");
        return false;
    case ShowTotalBalances:
        PrintTotalBalancesTable(vClientsRecords);
        return false;
    case BackToMainMenu:
        return true;
    }
}

void PlayBankProjectFromTransactionsMenu(vector<stBankClientInfo>& vClientsRecords)
{
    if (CheckPermissionAccess(enMainMenuPermissions::pWorkOnTransactionsMenu) == false)
    {
        ShowAccessDeniedScreen();
        GoBackToMenu("Press any key to go back to Main Menu...");
        return;
    }
    bool ExitFromMenu;
    do
    {
        ShowTransactionsMenu();
        ExitFromMenu = PerformTransactionsMenuOption(ReadTransactionsMenuOption(), vClientsRecords);
    } while (ExitFromMenu != true);
}

// Play Manage Users Menu
void ShowManageUsersMenu()
{
    ClearScreen();
    BankSystemLogo();
    cout << "\t|----------------------------------------|\n";
    cout << "\t|           MANAGE USERS MENU            |\n";
    cout << "\t|----------------------------------------|\n";
    cout << "\t|   [1] Add New User                     |\n";
    cout << "\t|   [2] Show Users List                  |\n";
    cout << "\t|   [3] Update User Information          |\n";
    cout << "\t|   [4] Delete User                      |\n";
    cout << "\t|   [5] Search for User                  |\n";
    cout << "\t|   [6] Back to Main Menu                |\n";
    cout << "\t|----------------------------------------|\n\n";
    cout << "\tChoose what do you want from [1] to [6]: ";
}

enManageUsersMenuOptions ReadManageUsersMenuOption()
{
    short OptionNumber = 0;
    cin >> OptionNumber;
    while (!IsNumberInRange(OptionNumber, 1, 6))
    {
        PrintFailedMessage("Invalid Number! Enter number between [1] to [6]: ");
        cin >> OptionNumber;
    }
    return (enManageUsersMenuOptions)OptionNumber;
}

bool PerformManageUsersMenu(enManageUsersMenuOptions Option)
{
    switch (Option)
    {
    case RegisterNewUser:
        AddNewUsers();
        return false;
    case ShowUsers:
        ShowUsersTable();
        return false;
    case UpdateUserInfo:
        UpdateUserByUsername();
        GoBackToMenu("Press any key to go back to Manage Users Menu...");
        return false;
    case DeleteUser:
        DeleteUserByUsername();
        GoBackToMenu("Press any key to go back to Manage Users Menu...");
        return false;
    case FindUserInfo:
        FindUserByUsername();
        return false;
    case ReturnToMainMenu:
        return true;
    }
}

void PlayBankProjectFromManageUsersMenu()
{
    if (CheckPermissionAccess(enMainMenuPermissions::pWorkOnManageUsersMenu) == false)
    {
        ShowAccessDeniedScreen();
        GoBackToMenu("Press any key to go back to Main Menu...");
        return;
    }
    bool ExitFromMenu;
    do
    {
        ShowManageUsersMenu();
        ExitFromMenu = PerformManageUsersMenu(ReadManageUsersMenuOption());
    } while (ExitFromMenu != true);
}