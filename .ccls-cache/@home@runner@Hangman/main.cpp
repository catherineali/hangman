#include <bits/stdc++.h>
using namespace std;

int n, k, score, known;  bool canGuess;
string info, ans = "";
char vow[11] = { 'a', 'e', 'f', 'h', 'i', 'l', 'm', 'n', 'o', 's', 'x' }; //letters that start with vowels
vector <int> spaces;

int process(char c) {
  if (info.size() == 1) return 0;
  int i = 0, cnt = 0;
  while (i < info.size()) {
    string num = ""; 
    if (!(info[i] >= '0' && info[i] <= '9')) { i++; continue; }
    int j = i;
    while (info[j] >= '0' && info[j] <= '9') num += info[j++];
    i = j + 1; //2 if space between , and next number
    int ind = stoi(num);
    ans[ind - 1] = c; 
    cnt++;
    if (c == ' ') spaces.push_back(ind - 1);
  } 
  known += cnt; return cnt;
}

double entropy(double p) {
  return p * log(1 / p) + (1 - p) * log(1 / (1 - p));
}

char bestGuess() {
  char x; double maxH = 0.0;
  for (char c = 'A'; c <= 'Z'; c++) {
    double totalH = 0.0;
    for (int i = 0; i < spaces.size() - 1; i++) { //each word from index spaces[i] + 1 to spaces[i + 1] - 1
      for (int j = 0; j < spaces[i + 1] - spaces[i] - 1; j++) 
        if (ans[spaces[i] + 1 + j] == '`') {
          double p; //probability that c is in this pos, given previous answers and length spaces[i + 1] - spaces[i] - 1
          totalH += entropy(p);
        }
      //totalH += entropy(v);
    }
    if (totalH >= maxH) x = c, maxH = totalH;
  }
  return x;
  /*char x = 'A';
  for (int i = 0; i < k - 2; i++) x++;
  return x;*/
  //if entropy of guessing whole thing > entropy of 3 guesses, then canGuess = true
  //https://www.wordfrequency.info/samples.asp
}

int main() {
  cout << "How many characters are in this phrase?" << endl;
  string s; getline(cin, s); n = stoi(s);
  for (int i = 0; i < n; i++) ans += "`"; // placeholder for unknown
  cout << "Guess #1: Is there a blank? \n";
  getline(cin, info);
  spaces.push_back(-1); process(' '); spaces.push_back(n);
  k = 2;
  while (known <= n) {
    score++;
    cout << "Guess #" << k << ": ";
    if (known >= n || canGuess) {
      cout << ans << endl;
      getline(cin, info);
      if (info == "Y") return 0;
      score += 2; 
    }
    else {
      char c = bestGuess(); 
      cout << "Is there a";
      for (int i = 0; i < 11; i++) 
        if (c == vow[i]) {
          cout << "n";
          break;
        }
      cout << " " << c << "?" << endl;
      getline(cin, info);
      process(c);
    }
    k++; 
  }
  cout << "Score: " << score << endl;
}