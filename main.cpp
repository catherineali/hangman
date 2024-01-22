#include <bits/stdc++.h>
using namespace std;

int n, k = 2, score = 3, known, lie, punc, puncInd, hasReset;
char wrong[2], punctuation[3] = {'?', '!', ','};
bool canGuess;
string info, ans = "", guessAns = "";
int vow[26],
    let[26] = {4, 0, 17, 8, 14, 19, 13, 18, 11, 2, 20, 3, 15, 12,
               7, 6, 1, 5, 24, 22, 10, 21, 23, 25, 9, 16}; // common letters
vector<int> spaces;
int a[20], b[20], done[26], p[26];
vector<string> w;
set<string> wrongGuess;
map<char, string> m;

double entropy(double p) {
  if (p == 0 || p == 1)
    return p;
  return p * log(1 / p) + (1 - p) * log(1 / (1 - p));
}

void setup() {
  for (int i = 0; i < n; i++)
    ans += "`", guessAns += '`'; // placeholder for unknown

  vow[0]++, vow[4]++, vow[5]++, vow[7]++, vow[8]++, vow[11]++, vow[12]++,
      vow[13]++, vow[14]++, vow[17]++, vow[18]++,
      vow[23]++; //'a', 'e', 'f', 'h', 'i', 'l', 'm', 'n', 'o', 'r', 's', 'x'
                 //(letters that start with vowels)

  ifstream fin("words.txt");
  int l = 1, i = 0;
  a[1] = 0;
  for (int j = 0; j < 9487; j++) {
    string s;
    fin >> s;
    w.push_back(s);
    if (s.size() == l)
      b[l] = i;
    else
      a[++l] = i;
    i++;
  }

  m.insert({'.', "period"}), m.insert({',', "comma"}),
      m.insert({'?', "question mark"}), m.insert({'!', "exclamation mark"});
}

void reset() {
  hasReset = 1;
  if (ans[ans.size() - 1] == '`')
    punc = 1;
  for (int i = 0; i < 26; i++)
    done[i] = 0;
}

int process(char c) {
  if (lie) {
    char realLie = c;
    if (info.size() == 1)
      realLie = wrong[0];
    else
      realLie = wrong[1];
    known -= p[(int)(realLie - 'A')];
    lie = 0;
    return 0;
  }

  if (c >= 'A' && c <= 'Z')
    done[(int)(c - 'A')] = 1;
  if (info.size() == 1)
    return 0;
  int i = 0, cnt = 0;
  while (i < info.size()) {
    string num = "";
    if (!(info[i] >= '0' && info[i] <= '9')) {
      i++;
      continue;
    }
    int j = i;
    while (info[j] >= '0' && info[j] <= '9')
      num += info[j++];
    i = j + 1; // 2 if space between , and next number
    int ind = stoi(num);
    ind--;
    if (ans[ind] == '`')
      ans[ind] = c;
    else { // ask again
      lie = 1;
      wrong[0] = c, wrong[1] = ans[ind];
    }
    cnt++;
    if (!(c >= 'A' && c <= 'Z')) {
      spaces.push_back(ind);
      sort(spaces.begin(), spaces.end());
    }
  }
  known += cnt;
  if (c >= 'A' && c <= 'Z')
    p[(int)(c - 'A')] = cnt;
  return cnt;
}

bool updateCanGuess() {
  for (int i = 0; i < n; i++)
    if (!(ans[i] >= 'A' && ans[i] <= 'Z'))
      guessAns[i] = ans[i];
  int totalPoss = 1;
  canGuess = 0;
  for (int i = 0; i < spaces.size() - 1; i++) {
    int wordLen = spaces[i + 1] - spaces[i] - 1;
    vector<int> pos;
    for (int j = 0; j < wordLen; j++)
      if (ans[spaces[i] + 1 + j] != '`')
        pos.push_back(j);
    int cnt = 0, bad = 0, guessed = 0;
    for (int k = a[wordLen]; k <= b[wordLen]; k++) {
      bad = 0;
      string s = w[k];
      for (int m : pos)
        if (s[m] - 'a' + 'A' != ans[spaces[i] + 1 + m])
          bad = 1;
      if (pos.size() >= 1 && pos.size() >= (wordLen - 2) && !bad) {
        cnt++;
        if (!guessed)
          for (int j = 0; j < wordLen; j++)
            guessAns[spaces[i] + 1 + j] = s[j] - 'a' + 'A', guessed = 1;
      }
    }
    if (wordLen > 0)
      totalPoss *= cnt;
  }
  if (totalPoss == 1 && wrongGuess.find(guessAns) == wrongGuess.end()) {
    canGuess = 1;
    return 1;
  }
  return 0;
}

char bestGuess() {
  if (lie)
    return wrong[0];

  char x = '`';
  double maxH = 0.0, totalH = 0.0;

  if (punc && puncInd < 3)
    return punctuation[puncInd++];

  for (char c = 'Z'; c >= 'A'; c--) {
    if (done[(int)(c - 'A')])
      continue;
    totalH = 0.0;
    for (int i = 0; i < spaces.size() - 1;
         i++) { // each word from index spaces[i] + 1 to spaces[i + 1] - 1
      int wordLen = spaces[i + 1] - spaces[i] - 1;

      vector<int> pos;
      for (int j = 0; j < wordLen; j++)
        if (ans[spaces[i] + 1 + j] != '`')
          pos.push_back(j);

      for (int j = 0; j < wordLen; j++) {
        int num = 0, den = 0;
        if (ans[spaces[i] + 1 + j] == '`') {
          double p = -1; // probability that c is in this pos, given previous
                         // answers and wordLen
          for (int k = a[wordLen]; k <= b[wordLen];
               k++) { // k: index of word with correct length in vector w
            p = -1;
            string s = w[k];
            for (int m : pos)
              if (s[m] - 'a' + 'A' != ans[spaces[i] + 1 + m])
                p = 0;
            if (p != 0) {
              den++;
              if (s[j] - 'a' + 'A' == c)
                num++;
            }
          }
          if (num == 0)
            p = 0;
          else
            p = (num + 0.0) / den;
          totalH += entropy(p);
        }
      }
    }
    if (totalH >= maxH)
      x = c, maxH = totalH;
  }
  if (maxH == 0) {
    int i = 0;
    while (i < 26 && done[let[i]])
      i++;
    x = 'A' + let[i];
    if (!hasReset) reset();
  }
  int allGuessed = 1;
  for (int i = 0; i < 26; i++)
    if (!done[i])
      allGuessed = 0;
  if (allGuessed)
    x = '`';
  return x;
}

int main() {
  cout << "How many characters are in this phrase?" << endl;
  string s;
  getline(cin, s);
  n = stoi(s);

  setup();

  cout << "Guess #1: Is there a blank?\n";
  spaces.push_back(-1);
  spaces.push_back(n);
  getline(cin, info);
  process(' ');

  cout << "Guess #2: Is there a period?\n";
  getline(cin, info);
  process('.');

  while (1) {
    score++, k++;
    cout << "Guess #" << k << ": ";
    if (known >= n || canGuess) {
      if (known >= n)
        cout << ans << endl;
      else if (canGuess)
        cout << guessAns << endl;
      getline(cin, info);
      if (info == "Y")
        break;
      wrongGuess.insert(guessAns);
      score += 2, punc = 1, canGuess = 0;
      for (int i = 0; i < 26; i++)
        done[i] = 0;
    } else {
      cout << "Is there a";
      char c = bestGuess();
      if (c == '`') {
        reset();
        c = bestGuess();
      }
      if (c >= 'A' && c <= 'Z' && vow[(int)(c - 'A')])
        cout << "n";
      string s = "";
      if (c >= 'A' && c <= 'Z')
        s += c;
      else
        s = m[c];
      cout << " " << s << "?" << endl;
      getline(cin, info);
      process(c);
      updateCanGuess();
    }
  }
  cout << "Score: " << score << endl;
}