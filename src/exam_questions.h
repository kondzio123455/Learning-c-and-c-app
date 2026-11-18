#ifndef EXAM_QUESTIONS_H
#define EXAM_QUESTIONS_H

// Exam questions: the Lab 1 and Lab 2 ("Coding Exam 2 Preparation") functions,
// with a premade main in the lecturer's style that prints yes or no for each test.

enum ExamPart { PART_LAB1, PART_LAB2, PART_LISTS };

struct ExamQuestion {
    ExamPart part;
    const char *source;        // e.g. "Lab 2 Q3"
    const char *title;
    const char *description;   // the lab's own words
    const char *support;       // premade code above your function (structs, classes, test helpers)
    const char *starter;       // what the editor starts with: the prototype and a stub, like the lab
    const char *mainBody;      // the premade main's tests
    const char *answer;        // one correct answer (shown after the exam)
    bool noIndexing;           // Lab 2 rule: no arr[i]
};

const int EXAM_QUESTION_COUNT = 28;
ExamQuestion *getExamQuestion(int index);

// The whole program to compile: includes, premade code, your code, then main.
// #line markers make compiler errors say "your_code:5" instead of a line in the big file.
void buildExamProgram(ExamQuestion *q, const char *yourCode, char *out, int size);

// The premade code as you see it (everything except your function)
void premadeCodeText(ExamQuestion *q, char *out, int size);

int countTests(ExamQuestion *q);             // how many yes/no lines main prints
bool examUsesIndexing(const char *code);         // Lab 2 rule check: arr[i] outside comments and strings

#endif
