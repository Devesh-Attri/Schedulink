#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <functional>
#include <fstream>
#include <sstream>

using namespace std;

class Task {
public:
    string name;
    string description;
    string due_date;
    int priority;
    set<string> dependencies;
    bool completed;
    string remarks;

    Task(const string& n, const string& desc, const string& date, int prio)
        : name(n), description(desc), due_date(date), priority(prio), completed(false) {}
};

class Employee {
public:
    string name;
    string department;
    vector<Task> tasks;

    Employee(const string& n, const string& dept) : name(n), department(dept) {}
};

class TeamMember {
public:
    string employeeID;
    string department;
    string name;

    TeamMember(const string& id, const string& dept, const string& n)
        : employeeID(id), department(dept), name(n) {}
};

class TaskList {
private:
    map<string, vector<Task>> employeeTasks;
    vector<Task> tasks;
    priority_queue<Task, vector<Task>, function<bool(const Task&, const Task&)>> taskQueue;
    vector<TeamMember> teamMembers;

public:
    TaskList() : taskQueue([](const Task& t1, const Task& t2) { return t1.priority < t2.priority; }) {}

    void addTask(const Task& task) {
        tasks.push_back(task);
        taskQueue.push(task);
    }

    void assignTaskToEmployee(const string& employeeID, const Task& task) {
        employeeTasks[employeeID].push_back(task);
    }

    void removeTask(const string& taskName) {
        tasks.erase(remove_if(tasks.begin(), tasks.end(), [&](const Task& task) {
            return task.name == taskName;
        }), tasks.end());
    }

    void updateTask(const string& taskName, const Task& newTask) {
        for (auto& task : tasks) {
            if (task.name == taskName) {
                task = newTask;
                break;
            }
        }
    }

    void displayTasks() {
        for (const Task& task : tasks) {
            cout << "Task: " << task.name << endl;
            cout << "Description: " << task.description << endl;
            cout << "Due Date: " << task.due_date << endl;
            cout << "Priority: " << task.priority << endl;
            cout << "---------------------" << endl;
        }
    }

    void displayHighestPriorityTasks() {
        if (tasks.empty()) {
            cout << "No tasks assigned yet." << endl;
            return;
        }

        cout << "Tasks with the highest priority:" << endl;
        while (!taskQueue.empty()) {
            Task task = taskQueue.top();
            cout << "Task: " << task.name << endl;
            cout << "Description: " << task.description << endl;
            cout << "Due Date: " << task.due_date << endl;
            cout << "Priority: " << task.priority << endl;
            cout << "---------------------" << endl;
            taskQueue.pop();
        }
    }

    vector<Task> getTasksByDueDate() {
        vector<Task> tasksByDueDate = tasks;
        sort(tasksByDueDate.begin(), tasksByDueDate.end(), [](const Task& t1, const Task& t2) {
            return t1.due_date < t2.due_date;
        });
        return tasksByDueDate;
    }

    vector<Task> getTasksForEmployee(const string& employeeID) {
        if (employeeTasks.find(employeeID) != employeeTasks.end()) {
            // Get the tasks for the employee
            vector<Task> employeeTasksList = employeeTasks[employeeID]; // Make a copy
            // Sort the tasks by priority (high to low)
            sort(employeeTasksList.begin(), employeeTasksList.end(), [](const Task& t1, const Task& t2) {
                return t1.priority > t2.priority;
            });

            return employeeTasksList;
        }
        return {};
    }

    void printTeamMembersFromCSV(const string& department) {
        // Construct the filename based on the department
        string filename = department + "_team_members.csv";

        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error opening file: " << filename << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            vector<string> tokens;
            string token;

            while (getline(iss, token, ',')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 3 && tokens[1] == department) {
                TeamMember member(tokens[0], tokens[1], tokens[2]);
                teamMembers.push_back(member);
                cout << "Employee ID: " << member.employeeID << ", Name: " << member.name << endl;
            }
        }

        file.close();
    }
};

// Function to mark a task as completed and add remarks
void markTaskAsCompleted(Task& task) {
    task.completed = true;
    cout << "Task marked as completed." << endl;

    // Allow admin or employee to add remarks
    cout << "Enter remarks: ";
    cin.ignore();
    getline(cin, task.remarks);
}

// Function to manage tasks for employees
void employeeTaskManagement(const string& employeeID, TaskList& taskList) {
    while (true) {
        cout << "1. Display Tasks" << endl;
        cout << "2. Mark Task as Completed" << endl;
        cout << "3. Add a Remark" << endl;
        cout << "4. Return to the main menu" << endl;
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            // Display tasks for the selected employee
            cout << "Tasks for Employee with ID: " << employeeID << endl;
            const vector<Task> tasks = taskList.getTasksForEmployee(employeeID);
            if (tasks.empty()) {
                cout << "No tasks assigned to this employee yet." << endl;
            } else {
                for (const Task& task : tasks) {
                    cout << "Task: " << task.name << endl;
                    cout << "Description: " << task.description << endl;
                    cout << "Due Date: " << task.due_date << endl;
                    cout << "Priority: " << task.priority << endl;
                    cout << "Status: " << (task.completed ? "Completed" : "Not Completed") << endl;
                    cout << "Remarks: " << task.remarks << endl;
                    cout << "---------------------" << endl;
                }
            }
        } else if (choice == 2) {
            // Mark a task as completed
            cout << "Enter the task name to mark as completed: ";
            string taskName;
            cin.ignore();
            getline(cin, taskName);

            // Find the task to mark as completed by name
            vector<Task> tasks = taskList.getTasksForEmployee(employeeID);
            for (Task& task : tasks) {
                if (task.name == taskName) {
                    if (task.completed) {
                        cout << "Task is already marked as completed." << endl;
                    } else {
                        markTaskAsCompleted(task);
                    }
                    break;
                }
            }
        } else if (choice == 3) {
            // Add a remark to a task
            cout << "Enter the task name to add a remark: ";
            string taskName;
            cin.ignore();
            getline(cin, taskName);

            // Find the task to add a remark by name
            vector<Task> tasks = taskList.getTasksForEmployee(employeeID);
            for (Task& task : tasks) {
                if (task.name == taskName) {
                    cout << "Enter the remark: ";
                    getline(cin, task.remarks);
                    cout << "Remark added for Task: " << taskName << endl;
                    break;
                }
            }
        } else if (choice == 4) {
            break; // Return to the main menu
        } else {
            cout << "Invalid choice. Please enter a valid option." << endl;
        }
    }
}

int main() {
    TaskList taskList;

    Task sampleTask1("Task 1", "Description for Task 1", "2023-11-01", 3);
    Task sampleTask2("Task 2", "Description for Task 2", "2023-11-05", 2);
    Task sampleTask3("Task 3", "Description for Task 3", "2023-11-03", 1);

    // Adding tasks to the task list
    taskList.addTask(sampleTask1);
    taskList.addTask(sampleTask2);
    taskList.addTask(sampleTask3);

    while (true) {
        cout << "-------Welcome to Schedulink!!!-------" << endl;
        cout << "Are you an Admin or an Employee?" << endl;
        cout << "Press 1 for Admin" << endl;
        cout << "Press 2 for Employee" << endl;
        cout << "Press 3 to Exit" << endl;

        int userRole;
        cin >> userRole;

        if (userRole == 3) {
            break; // Exit the program
        } else if (userRole == 1) {
            // Admin functionality
            cout << "Select a department:" << endl;
            cout << "Press 1 for HR team" << endl;
            cout << "Press 2 for Technical Team" << endl;
            cout << "Press 3 for Marketing Team" << endl;
            int deptChoice;
            cout << "Enter your choice: " << endl;
            cin >> deptChoice;
            cin.ignore(); // Add this line to clear the newline character

            if (deptChoice >= 1 && deptChoice <= 3) {
                const string adminID = "Admin"; // Change this if you have specific admin IDs
                string department;

                // Determine the department based on the user's choice
                if (deptChoice == 1) {
                    department = "HR";
                } else if (deptChoice == 2) {
                    department = "Technical";
                } else if (deptChoice == 3) {
                    department = "Marketing";
                }

                cout << "Admin, " << adminID << ", of department " << department << endl;

                // Print the list of team members from the CSV file
                taskList.printTeamMembersFromCSV(department);

                while (true) {
                    int employeeChoice;
                    cout << "Select an employee to manage tasks (Enter 0 to return to the main menu): ";
                    cin >> employeeChoice;
                    cin.ignore(); // Clear the newline character

                    if (employeeChoice == 0) {
                        break; // Return to the main menu
                    } else if (employeeChoice >= 1 && employeeChoice <= 3) {
                        const string employeeID = "Employee " + to_string(employeeChoice);

                        while (true) {
                            cout << "1. Add Task" << endl;
                            cout << "2. Remove Task" << endl;
                            cout << "3. Update Task" << endl;
                            cout << "4. Display Tasks" << endl;
                            cout << "5. Go to list of teams." << endl;
                            cout << "Enter your choice: ";
                            int choice;
                            cin >> choice;

                            if (choice == 1) {
                                string name, description, due_date;
                                int priority;

                                cout << "Enter task name: ";
                                cin.ignore();
                                getline(cin, name);
                                cout << "Enter task description: ";
                                getline(cin, description);
                                cout << "Enter due date: ";
                                cin >> due_date;

                                // Prompt for priority within the specified range
                                do {
                                    cout << "Enter priority (1-5): ";
                                    cin >> priority;

                                    // Check if the priority is within the valid range
                                    if (priority < 1 || priority > 5) {
                                    cout << "Invalid priority. Please enter a priority between 1 and 5 (inclusive)." << endl;
                                    }
                                } while (priority < 1 || priority > 5);

                                Task newTask(name, description, due_date, priority);

                                // Add the task to the selected employee's task list
                                taskList.assignTaskToEmployee(employeeID, newTask);

                               cout << "Task assigned to Employee with ID: " << employeeID << endl;
                                } else if (choice == 2) {
                                cout << "Enter the task name to remove: ";
                                string taskName;
                                cin.ignore();
                                getline(cin, taskName);

                                // Remove the task for the selected employee
                                taskList.removeTask(taskName);

                                cout << "Task removed from Employee with ID: " << employeeID << endl;
                            } else if (choice == 3) {
                                cout << "Enter the task name to update: ";
                                string taskName;
                                cin.ignore();
                                getline(cin, taskName);

                                string name, description, due_date;
                                int priority;
                                cout << "Enter new task name: ";
                                cin.ignore();
                                getline(cin, name);
                                cout << "Enter new task description: ";
                                getline(cin, description);
                                cout << "Enter new due date: ";
                                cin >> due_date;
                                cout << "Enter new priority: ";
                                cin >> priority;

                                Task updatedTask(name, description, due_date, priority);

                                // Update the task for the selected employee
                                taskList.updateTask(taskName, updatedTask);

                                cout << "Task updated for Employee with ID: " << employeeID << endl;
                            } else if (choice == 4) {
                                // Display tasks for the selected employee
                                cout << "Tasks for Employee with ID: " << employeeID << endl;
                                const vector<Task>& tasks = taskList.getTasksForEmployee(employeeID);
                                if (tasks.empty()) {
                                    cout << "No tasks assigned to this employee yet." << endl;
                                } else {
                                    for (const Task& task : tasks) {
                                        cout << "Task: " << task.name << endl;
                                        cout << "Description: " << task.description << endl;
                                        cout << "Due Date: " << task.due_date << endl;
                                        cout << "Priority: " << task.priority << endl;
                                        cout << "---------------------" << endl;
                                    }
                                }
                            } else if (choice == 5) {
                                // Return to the employee selection
                                break;
                            }
                        }
                    } else {
                        cout << "Invalid choice. Please select a valid employee or enter 0 to return to the main menu." << endl;
                    }
                }
            } else {
                cout << "Invalid department choice. Please select a valid department." << endl;
            }
        } else if (userRole == 2) {
            cout << "Enter your employee ID: ";
            string employeeID;
            cin.ignore();
            getline(cin, employeeID);
            employeeTaskManagement(employeeID, taskList);
        } else {
            cout << "Invalid choice. Please select 1 for Admin or 2 for Employee." << endl;
        }
    }

    return 0;
}