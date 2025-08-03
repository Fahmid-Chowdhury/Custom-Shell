@echo off
echo Creating folders and files for testing...

REM Create folder
mkdir "my folder"

REM input.txt
echo banana > input.txt
echo apple >> input.txt
echo apple >> input.txt
echo banana >> input.txt
echo cherry >> input.txt

REM words.txt
echo hello > words.txt
echo world >> words.txt
echo hello >> words.txt
echo shell >> words.txt
echo unix >> words.txt

REM duplicates.txt
echo test > duplicates.txt
echo test >> duplicates.txt
echo test >> duplicates.txt
echo alpha >> duplicates.txt
echo beta >> duplicates.txt

REM my folder\test.txt
echo Inside quoted folder path. > "my folder\test.txt"

echo ✅ Files and folders created successfully!
