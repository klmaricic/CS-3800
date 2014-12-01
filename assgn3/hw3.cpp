//Authors: Kelsey Maricic, John Tegtmeyer

#include <iostream>
#include <signal.h>
#include <sys/socket.h>
using namespace std;

struct client {
  string userName;
  int clientId = 0;
} user;

int main()
{
  string hostname;
  

  cout << "Welcome! Please enter a hostname for the server" << endl;
  cin >> hostname;

  cout <<"What username would you like to use?" << endl;
  cin >> user.userName;
}
