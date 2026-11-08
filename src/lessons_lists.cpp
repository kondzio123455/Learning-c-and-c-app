#include "questions.h"

// ============================================================
// Chapter 7: the Pointers & Linked List lecture and Lab 2 Part 2.
// Chapter 8: Lab 1 review (Coding Exam 1, Q13 onwards).
// ============================================================

void setupListLessons() {
    Level *level;
    Task *task;

    // =========================================================
    // Chapter 7: Linked lists
    // =========================================================

    // ---------- Level 39 ----------
    level = startLevel(39, 7, "Nodes", "Node", "Pointers & Linked List lecture: Basic Structure of a Node");
    level->explanation = "A linked list is made of nodes. Each node holds its data and a pointer to the next node, and if that "
                         "pointer is nullptr it's the end of the list. New nodes are made with new, and you reach their fields "
                         "with -> because newNode is a pointer. (The lecture's first Node calls the pointer next; the full "
                         "program calls it nextPtr. Same idea.)";
    level->example = "Node* newNode = new Node();\n"
                     "newNode->nextPtr = nullptr;   // nothing after it yet";
    level->inGame = "The shot log under the table is a linked list: one node per shot.";
    level->slideTitle = "Pointers & Linked List lecture: Basic Structure of a Node";
    level->slideText = "A linked list is a dynamic data structure where each element (node) contains:\n"
                       "- Data (the actual information)\n"
                       "- A pointer to the next node in the list.\n"
                       "\n"
                       "struct Node {\n"
                       "    int data;     // Stores data\n"
                       "    Node* next;   // Pointer to the next node\n"
                       "};\n"
                       "If next is nullptr, it indicates the end of the list.\n"
                       "(The full program later in the notes calls it nextPtr.)";

    task = startTask(level, 0, TASK_TYPE, "Store newData in the new node.",
                     "void InsertAtEnd(Node** ptrToHeadPtr, int newData) {\n"
                     "    Node* newNode = new Node();\n"
                     "    ____\n"
                     "    newNode->nextPtr = nullptr;\n"
                     "    // ...\n"
                     "}");
    addAnswer(task, "newNode->data = newData;");
    addAnswer(task, "(*newNode).data = newData;");
    addMistake(task, "newNode.data = newData;", "newNode is a pointer, so . doesn't work on it.");
    addMistake(task, "data = newData;", "data belongs to the node, so you have to reach it through newNode.");
    addMistake(task, "newNode->data == newData;", "== compares. To store a value, use =.");
    setHints(task, "newNode is a pointer to a Node.",
                   "Reach a member through a pointer with ->.",
                   "Copy the style of the line below the gap.");

    task = startTask(level, 1, TASK_TYPE, "Finish the Node struct: add the pointer to the next node, called nextPtr.",
                     "struct Node {\n"
                     "    int data;   // Stores data\n"
                     "    ____        // Pointer to the next node\n"
                     "};");
    addAnswer(task, "Node* nextPtr;");
    addAnswer(task, "struct Node* nextPtr;");
    addMistake(task, "Node nextPtr;", "A Node can't contain a whole Node inside it (it would never end). Store a pointer to the next one.");
    addMistake(task, "int* nextPtr;", "The next thing in the list is a Node, so it's a pointer to a Node.");
    setHints(task, "It points at the next node.",
                   "So its type is 'pointer to Node'.",
                   "Node* ...;");

    // ---------- Level 40 ----------
    level = startLevel(40, 7, "Walking a list", "PrintList", "Pointers & Linked List lecture: Traversing the List (PrintList)");
    level->explanation = "To visit every node, start at the head and keep following nextPtr until you reach nullptr. Nodes aren't "
                         "next to each other in memory like an array, so ++ won't move you along: you have to follow the links.";
    level->example = "Node* curNodePtr = headPtr;   // start at the first node\n"
                     "// ... then follow the links one by one";
    level->inGame = "The shot log is drawn by walking its list like this.";
    level->slideTitle = "Pointers & Linked List lecture: Traversing the List";
    level->slideText = "void PrintList(Node* headPtr) {\n"
                       "    Node* curNodePtr = headPtr;\n"
                       "    while (curNodePtr != nullptr) {\n"
                       "        cout << curNodePtr->data << \" \";\n"
                       "        curNodePtr = curNodePtr->nextPtr;\n"
                       "    }\n"
                       "    cout << endl;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Type the line that moves curNodePtr on to the next node.",
                     "void PrintList(Node* headPtr) {\n"
                     "    Node* curNodePtr = headPtr;\n"
                     "    while (curNodePtr != nullptr) {\n"
                     "        cout << curNodePtr->data << \" \";\n"
                     "        ____\n"
                     "    }\n"
                     "    cout << endl;\n"
                     "}");
    addAnswer(task, "curNodePtr = curNodePtr->nextPtr;");
    addAnswer(task, "curNodePtr = (*curNodePtr).nextPtr;");
    addMistake(task, "curNodePtr++;", "Nodes aren't next to each other in memory like an array, so ++ won't reach the next one.");
    addMistake(task, "curNodePtr->nextPtr = curNodePtr;", "That changes the node's link. curNodePtr itself has to move.");
    addMistake(task, "curNodePtr = nextPtr;", "nextPtr belongs to a node, so reach it through curNodePtr.");
    setHints(task, "curNodePtr has to move to the next node.",
                   "Each node stores where the next one is.",
                   "curNodePtr = the current node's nextPtr.");

    task = startTask(level, 1, TASK_TYPE, "Type the while line that keeps going until the end of the list.",
                     "void PrintList(Node* headPtr) {\n"
                     "    Node* curNodePtr = headPtr;\n"
                     "    ____\n"
                     "        cout << curNodePtr->data << \" \";\n"
                     "        curNodePtr = curNodePtr->nextPtr;\n"
                     "    }\n"
                     "}");
    addAnswer(task, "while (curNodePtr != nullptr) {");
    addAnswer(task, "while (curNodePtr != NULL) {");
    addAnswer(task, "while (curNodePtr) {");
    addMistake(task, "while (curNodePtr->nextPtr != nullptr) {", "That stops one node early, so the last node isn't printed.");
    addMistake(task, "while (curNodePtr == nullptr) {", "That only runs when the list is empty.");
    addMistake(task, "while (headPtr != nullptr) {", "headPtr never changes, so this would never stop.");
    setHints(task, "The end of the list is a nullptr.",
                   "Check curNodePtr, which is the one that moves.",
                   "Keep going while curNodePtr isn't nullptr.");

    // ---------- Level 41 ----------
    level = startLevel(41, 7, "InsertAtEnd and Node**", "InsertEnd", "Pointers & Linked List lecture: Inserting a Node at the End");
    level->explanation = "InsertAtEnd takes Node** ptrToHeadPtr: a pointer to the caller's head pointer (Level 6's double "
                         "indirection). That lets it change the head when the list is empty. Otherwise it walks to the last node "
                         "and links the new node after it.";
    level->example = "Node* headPtr = nullptr;\n"
                     "InsertAtEnd(&headPtr, 10);   // pass the ADDRESS of headPtr,\n"
                     "InsertAtEnd(&headPtr, 20);   // so the function can change it";
    level->inGame = "Every shot is added to the end of the shot log.";
    level->slideTitle = "Pointers & Linked List lecture: Inserting a Node at the End";
    level->slideText = "void InsertAtEnd(Node** ptrToHeadPtr, int newData) {\n"
                       "    Node* newNode = new Node();\n"
                       "    newNode->data = newData;\n"
                       "    newNode->nextPtr = nullptr;\n"
                       "    if (*ptrToHeadPtr == nullptr) {\n"
                       "        *ptrToHeadPtr = newNode;\n"
                       "        return;\n"
                       "    }\n"
                       "    Node* curNodePtr = *ptrToHeadPtr;\n"
                       "    while (curNodePtr->nextPtr != nullptr) {\n"
                       "        curNodePtr = curNodePtr->nextPtr;\n"
                       "    }\n"
                       "    curNodePtr->nextPtr = newNode;\n"
                       "}\n"
                       "\n"
                       "main:   Node* headPtr = nullptr;\n"
                       "        InsertAtEnd(&headPtr, 10); // Insert 10 at the end";

    task = startTask(level, 0, TASK_TYPE, "The list is empty. Type the line that makes newNode the head.",
                     "void InsertAtEnd(Node** ptrToHeadPtr, int newData) {\n"
                     "    Node* newNode = new Node();\n"
                     "    newNode->data = newData;\n"
                     "    newNode->nextPtr = nullptr;\n"
                     "    if (*ptrToHeadPtr == nullptr) {\n"
                     "        ____\n"
                     "        return;\n"
                     "    }\n"
                     "    // ... otherwise walk to the end ...\n"
                     "}");
    addAnswer(task, "*ptrToHeadPtr = newNode;");
    addMistake(task, "ptrToHeadPtr = newNode;", "That only changes the function's own copy of the pointer. The caller's head has to change.");
    addMistake(task, "**ptrToHeadPtr = newNode;", "**ptrToHeadPtr would be a whole Node, not the head pointer.");
    addMistake(task, "Node* headPtr = newNode;", "That makes a new local variable. The caller's head is reached through ptrToHeadPtr.");
    setHints(task, "*ptrToHeadPtr is the caller's head pointer.",
                   "An empty list's head should become the new node.",
                   "Set the head pointer through ptrToHeadPtr.");

    task = startTask(level, 1, TASK_TYPE, "curNodePtr is now the last node. Type the line that links newNode after it.",
                     "    Node* curNodePtr = *ptrToHeadPtr;\n"
                     "    while (curNodePtr->nextPtr != nullptr) {\n"
                     "        curNodePtr = curNodePtr->nextPtr;\n"
                     "    }\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "curNodePtr->nextPtr = newNode;");
    addMistake(task, "curNodePtr = newNode;", "That only moves your local pointer. The last node's link has to change.");
    addMistake(task, "newNode->nextPtr = curNodePtr;", "That's the wrong direction: the last node should point to the new one.");
    addMistake(task, "*ptrToHeadPtr = newNode;", "That would replace the whole list with one node.");
    setHints(task, "The last node's nextPtr is nullptr at the moment.",
                   "It should point at the new node instead.",
                   "curNodePtr->nextPtr = ...;");

    // ---------- Level 42 ----------
    level = startLevel(42, 7, "DeleteWithValue", "Delete", "Pointers & Linked List lecture: DeleteWithValue");
    level->explanation = "To delete a node, find it while remembering the node before it (prev). Then unlink it by making prev point "
                         "past it, and free it with delete. If the head itself holds the value, the head has to move to the next "
                         "node, which is why the function takes Node **.";
    level->example = "Node* prev = nullptr;\n"
                     "while (curNodePtr != nullptr && curNodePtr->data != value) {\n"
                     "    prev = curNodePtr;\n"
                     "    curNodePtr = curNodePtr->nextPtr;\n"
                     "}";
    level->inGame = "Not used by the game, but it's in the linked list lecture.";
    level->slideTitle = "Pointers & Linked List lecture: DeleteWithValue";
    level->slideText = "void DeleteWithValue(Node **ptrToHeadPtr, int value) {\n"
                       "  if (*ptrToHeadPtr == nullptr) { cout << \"List is empty.\" << endl; return; }\n"
                       "  Node* curNodePtr = *ptrToHeadPtr;\n"
                       "  // If the head node itself holds the value to be deleted\n"
                       "  if (curNodePtr != nullptr && curNodePtr->data == value) {\n"
                       "    *ptrToHeadPtr = curNodePtr->nextPtr; // Change head to the next node\n"
                       "    delete curNodePtr;                   // Free memory\n"
                       "    return;\n"
                       "  }\n"
                       "  // Search for the node, keeping track of the previous node\n"
                       "  Node* prev = nullptr;\n"
                       "  while (curNodePtr != nullptr && curNodePtr->data != value) {\n"
                       "    prev = curNodePtr;\n"
                       "    curNodePtr = curNodePtr->nextPtr;\n"
                       "  }\n"
                       "  if (curNodePtr == nullptr) { cout << \"Value not found in the list.\" << endl; return; }\n"
                       "  prev->nextPtr = curNodePtr->nextPtr;   // Unlink the node from the linked list\n"
                       "  delete curNodePtr;                     // Free memory\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "The head node holds the value. Type the line that changes the head to the next node.",
                     "Node* curNodePtr = *ptrToHeadPtr;\n"
                     "// If the head node itself holds the value to be deleted\n"
                     "if (curNodePtr != nullptr && curNodePtr->data == value) {\n"
                     "    ____   // Change head to the next node\n"
                     "    delete curNodePtr;   // Free memory\n"
                     "    return;\n"
                     "}");
    addAnswer(task, "*ptrToHeadPtr = curNodePtr->nextPtr;");
    addMistake(task, "ptrToHeadPtr = curNodePtr->nextPtr;", "That only changes the function's own copy. The caller's head has to change.");
    addMistake(task, "*ptrToHeadPtr = curNodePtr;", "curNodePtr is the node being deleted. The head has to move past it.");
    addMistake(task, "curNodePtr = curNodePtr->nextPtr;", "That only moves your local pointer. The caller's head has to change.");
    setHints(task, "The head node is about to be deleted.",
                   "So the head must become the node after it.",
                   "Change the caller's head through ptrToHeadPtr.");

    task = startTask(level, 1, TASK_TYPE, "curNodePtr holds the value and prev is the node before it. Unlink curNodePtr.",
                     "// Unlink the node from the linked list\n"
                     "____\n"
                     "// Free memory\n"
                     "delete curNodePtr;");
    addAnswer(task, "prev->nextPtr = curNodePtr->nextPtr;");
    addMistake(task, "prev->nextPtr = curNodePtr;", "That still points at the node that's being deleted.");
    addMistake(task, "curNodePtr->nextPtr = prev->nextPtr;", "That changes the node being deleted. It's prev's link that has to skip over it.");
    addMistake(task, "prev = curNodePtr->nextPtr;", "That only moves your local pointer. prev's link has to change.");
    setHints(task, "prev's link has to skip over curNodePtr.",
                   "The node after curNodePtr is curNodePtr->nextPtr.",
                   "prev->nextPtr = ...;");

    // ---------- Level 43 ----------
    level = startLevel(43, 7, "clear(): freeing a list", "clear", "Pointers & Linked List lecture: the LinkedList class (clear and destructor)");
    level->explanation = "To free a whole list, walk it and delete each node. Save the node in temp, move on to the next node FIRST, "
                         "then delete temp. If you deleted the current node first, you couldn't follow its next pointer any more. "
                         "At the end, set head to nullptr. The class's destructor just calls clear().";
    level->example = "~LinkedList() {   // the destructor\n"
                     "    clear();\n"
                     "}";
    level->inGame = "When you start a new game, the shot log is cleared exactly like this.";
    level->slideTitle = "Pointers & Linked List lecture: the LinkedList class";
    level->slideText = "// Destructor to clean up memory\n"
                       "~LinkedList() {\n"
                       "    clear();\n"
                       "}\n"
                       "// Clear the entire list\n"
                       "void clear() {\n"
                       "    Node* current = head;\n"
                       "    while (current != nullptr) {\n"
                       "        Node* temp = current;\n"
                       "        current = current->next;\n"
                       "        delete temp;\n"
                       "    }\n"
                       "    head = nullptr;\n"
                       "}";

    task = startTask(level, 0, TASK_ORDER, "Build the loop body of clear(). Two of the lines are wrong, so leave them out.",
                     "void clear() {\n"
                     "    Node* current = head;\n"
                     "    while (current != nullptr) {\n"
                     "        ____\n"
                     "    }\n"
                     "    head = nullptr;\n"
                     "}");
    addTile(task, "delete temp;");
    addTile(task, "current++;");
    addTile(task, "Node* temp = current;");
    addTile(task, "delete current;");
    addTile(task, "current = current->next;");
    addAnswer(task, "Node* temp = current; current = current->next; delete temp;");
    addMistake(task, "Node* temp = current; delete temp; current = current->next;", "After delete temp, current points at freed memory, so current->next can't be read.");
    addMistake(task, "delete current; current = current->next;", "Once current is deleted, you can't follow its next pointer.");
    addMistake(task, "Node* temp = current; current++; delete temp;", "Nodes aren't next to each other in memory, so ++ doesn't reach the next one.");
    setHints(task, "Save the current node before moving on.",
                   "Move to the next node before deleting anything.",
                   "Save, move, then delete the saved node.");
    task->layout = "1  Node* temp = ...;\n"
                   "2  current = ...;\n"
                   "3  delete ...;";

    task = startTask(level, 1, TASK_TYPE, "Every node has been deleted. Type the line that marks the list as empty.",
                     "void clear() {\n"
                     "    Node* current = head;\n"
                     "    while (current != nullptr) {\n"
                     "        Node* temp = current;\n"
                     "        current = current->next;\n"
                     "        delete temp;\n"
                     "    }\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "head = nullptr;");
    addAnswer(task, "head = NULL;");
    addMistake(task, "delete head;", "head's node was already deleted in the loop, so this would be a double delete.");
    addMistake(task, "*head = nullptr;", "*head would be a Node, and it's been deleted. Change the pointer itself.");
    setHints(task, "head still holds the address of a deleted node.",
                   "An empty list's head points at nothing.",
                   "head = ...;");

    // ---------- Level 44 ----------
    level = startLevel(44, 7, "Doubly linked: prevPointer", "prevPtr", "Lab 2 Part 2: DoublyLinkedList and DoublyLinkedNode");
    level->explanation = "In a doubly linked list each node has two pointers: nextPointer to the node after it and prevPointer to "
                         "the node before it, so you can walk backwards too. Whenever you add a node, set both links.";
    level->example = "class DoublyLinkedNode {\n"
                     "public:\n"
                     "    int data;\n"
                     "    DoublyLinkedNode *prevPointer;\n"
                     "    DoublyLinkedNode *nextPointer;\n"
                     "};";
    level->inGame = "The shot log is a doubly linked list, so now you can scroll back through it with the Up and Down arrows.";
    level->slideTitle = "Lab 2, Part 2";
    level->slideText = "1. Implement the linkedlist list code from the lecture notes.\n"
                       "2. Create a new classes called DoublyLinkedList and a DoublyLinkedNode.\n"
                       "   DoublyLinkedNode has both a prevPointer and nextPointer\n"
                       "3. Implement and InsertAtBeginning , InsertAtEnd, and InsertAtPosition";

    task = startTask(level, 0, TASK_TYPE, "InsertAtEnd: newNode has just been linked after curNodePtr. Point newNode back at curNodePtr.",
                     "while (curNodePtr->nextPointer != nullptr) {\n"
                     "    curNodePtr = curNodePtr->nextPointer;\n"
                     "}\n"
                     "curNodePtr->nextPointer = newNode;\n"
                     "____");
    addAnswer(task, "newNode->prevPointer = curNodePtr;");
    addMistake(task, "curNodePtr->prevPointer = newNode;", "That's the wrong direction: the NEW node points back at curNodePtr.");
    addMistake(task, "newNode->nextPointer = curNodePtr;", "nextPointer goes forwards. Pointing back uses the other link.");
    addMistake(task, "newNode->prevPointer = newNode;", "A node can't be its own previous node.");
    setHints(task, "Every node needs its backwards link set too.",
                   "The node before newNode is curNodePtr.",
                   "newNode->prevPointer = ...;");

    task = startTask(level, 1, TASK_TYPE, "Print the list backwards: move curNodePtr back one node.",
                     "// start at the last node\n"
                     "while (curNodePtr != nullptr) {\n"
                     "    cout << curNodePtr->data << \" \";\n"
                     "    ____\n"
                     "}");
    addAnswer(task, "curNodePtr = curNodePtr->prevPointer;");
    addMistake(task, "curNodePtr = curNodePtr->nextPointer;", "nextPointer goes forwards. Going back uses the other link.");
    addMistake(task, "curNodePtr--;", "Nodes aren't in an array, so -- doesn't reach the previous one.");
    addMistake(task, "curNodePtr->prevPointer = curNodePtr;", "That changes a link. curNodePtr itself has to move.");
    setHints(task, "Follow the backwards link.",
                   "That's prevPointer.",
                   "curNodePtr = ...;");

    // ---------- Level 45 ----------
    level = startLevel(45, 7, "InsertAtBeginning", "InsertBeg", "Lab 2 Part 2; Pointers & Linked List lecture: insertAtBeginning");
    level->explanation = "To add a node at the front of a doubly linked list: point the new node forward at the old head, point the "
                         "old head back at the new node (if there is an old head), then make the new node the head.";
    level->example = "// the singly linked version from the lecture's class:\n"
                     "Node* newNode = new Node(newData);\n"
                     "newNode->next = head;\n"
                     "head = newNode;";
    level->inGame = "Not used by the game, but it's part of Lab 2.";
    level->slideTitle = "Lab 2 Part 2; Pointers & Linked List lecture: insertAtBeginning";
    level->slideText = "Lab 2, Part 2:\n"
                       "3. Implement and InsertAtBeginning , InsertAtEnd, and InsertAtPosition\n"
                       "\n"
                       "The lecture's (singly linked) class version:\n"
                       "// Insert a node at the beginning of the list\n"
                       "void insertAtBeginning(int newData) {\n"
                       "    Node* newNode = new Node(newData);\n"
                       "    newNode->next = head;\n"
                       "    head = newNode;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Type the line that points the new node forward at the old head.",
                     "void DoublyLinkedList::InsertAtBeginning(int newData) {\n"
                     "    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);\n"
                     "    ____\n"
                     "    if (head != nullptr) {\n"
                     "        head->prevPointer = newNode;\n"
                     "    }\n"
                     "    head = newNode;\n"
                     "}");
    addAnswer(task, "newNode->nextPointer = head;");
    addMistake(task, "head->nextPointer = newNode;", "That's the wrong direction: the new node points forward at head.");
    addMistake(task, "newNode = head;", "That throws away your new node.");
    addMistake(task, "newNode->prevPointer = head;", "The old head comes AFTER the new node, so it's the forward link.");
    setHints(task, "The new node goes in front of the old head.",
                   "So the new node's forward link leads to head.",
                   "newNode->nextPointer = ...;");

    task = startTask(level, 1, TASK_TYPE, "Type the line that points the old head back at the new node.",
                     "void DoublyLinkedList::InsertAtBeginning(int newData) {\n"
                     "    DoublyLinkedNode *newNode = new DoublyLinkedNode(newData);\n"
                     "    newNode->nextPointer = head;\n"
                     "    if (head != nullptr) {\n"
                     "        ____\n"
                     "    }\n"
                     "    head = newNode;\n"
                     "}");
    addAnswer(task, "head->prevPointer = newNode;");
    addMistake(task, "newNode->prevPointer = head;", "That's the wrong direction: the old head points BACK at the new node.");
    addMistake(task, "head = newNode;", "That happens on the last line. First link the old head back.");
    addMistake(task, "head->nextPointer = newNode;", "Going back uses the other link.");
    setHints(task, "The old head now has a node in front of it.",
                   "Its backwards link should lead to newNode.",
                   "head->prevPointer = ...;");

    // ---------- Level 46 ----------
    level = startLevel(46, 7, "InsertAtPosition", "InsertPos", "Lab 2 Part 2");
    level->explanation = "To insert in the middle, walk to the node just before the position (curNodePtr). The new node goes between "
                         "curNodePtr and the node after it, so four links change: two on the new node, and one on each "
                         "neighbour. Order matters: use curNodePtr->nextPointer before you change it.";
    level->example = "// before:  A <-> C\n"
                     "// after:   A <-> B <-> C";
    level->inGame = "Not used by the game - but with this you've done all of Lab 2. Well done!";
    level->slideTitle = "Lab 2, Part 2";
    level->slideText = "3. Implement and InsertAtBeginning , InsertAtEnd, and InsertAtPosition\n"
                       "\n"
                       "The lab doesn't give the code. In the middle of a doubly linked list the new\n"
                       "node goes between curNodePtr and the node after it, so four links change:\n"
                       "  newNode->prevPointer\n"
                       "  newNode->nextPointer\n"
                       "  the next node's prevPointer\n"
                       "  curNodePtr->nextPointer";

    task = startTask(level, 0, TASK_ORDER, "Put the four links in a working order. One line is wrong, so leave it out.",
                     "// curNodePtr is the node just before the position,\n"
                     "// and there IS a node after it\n"
                     "____");
    addTile(task, "curNodePtr->nextPointer = newNode;");
    addTile(task, "newNode->prevPointer = curNodePtr;");
    addTile(task, "curNodePtr->prevPointer = newNode;");
    addTile(task, "curNodePtr->nextPointer->prevPointer = newNode;");
    addTile(task, "newNode->nextPointer = curNodePtr->nextPointer;");
    // B and C must both come before D. A (newNode->prevPointer) can go anywhere.
    addAnswer(task, "newNode->prevPointer = curNodePtr; newNode->nextPointer = curNodePtr->nextPointer; curNodePtr->nextPointer->prevPointer = newNode; curNodePtr->nextPointer = newNode;");
    addAnswer(task, "newNode->nextPointer = curNodePtr->nextPointer; newNode->prevPointer = curNodePtr; curNodePtr->nextPointer->prevPointer = newNode; curNodePtr->nextPointer = newNode;");
    addAnswer(task, "newNode->nextPointer = curNodePtr->nextPointer; curNodePtr->nextPointer->prevPointer = newNode; newNode->prevPointer = curNodePtr; curNodePtr->nextPointer = newNode;");
    addAnswer(task, "newNode->nextPointer = curNodePtr->nextPointer; curNodePtr->nextPointer->prevPointer = newNode; curNodePtr->nextPointer = newNode; newNode->prevPointer = curNodePtr;");
    addAnswer(task, "newNode->prevPointer = curNodePtr; curNodePtr->nextPointer->prevPointer = newNode; newNode->nextPointer = curNodePtr->nextPointer; curNodePtr->nextPointer = newNode;");
    addAnswer(task, "curNodePtr->nextPointer->prevPointer = newNode; newNode->prevPointer = curNodePtr; newNode->nextPointer = curNodePtr->nextPointer; curNodePtr->nextPointer = newNode;");
    addAnswer(task, "curNodePtr->nextPointer->prevPointer = newNode; newNode->nextPointer = curNodePtr->nextPointer; newNode->prevPointer = curNodePtr; curNodePtr->nextPointer = newNode;");
    addAnswer(task, "curNodePtr->nextPointer->prevPointer = newNode; newNode->nextPointer = curNodePtr->nextPointer; curNodePtr->nextPointer = newNode; newNode->prevPointer = curNodePtr;");
    addMistake(task, "newNode->prevPointer = curNodePtr; curNodePtr->nextPointer = newNode; newNode->nextPointer = curNodePtr->nextPointer; curNodePtr->nextPointer->prevPointer = newNode;",
               "Once curNodePtr->nextPointer = newNode runs, the old next node can't be reached any more.");
    addMistake(task, "curNodePtr->nextPointer = newNode; newNode->prevPointer = curNodePtr; newNode->nextPointer = curNodePtr->nextPointer; curNodePtr->nextPointer->prevPointer = newNode;",
               "Once curNodePtr->nextPointer = newNode runs, the old next node can't be reached any more.");
    setHints(task, "Two links are on newNode, and one on each neighbour.",
                   "curNodePtr->nextPointer still leads to the old next node, so use it before changing it.",
                   "curNodePtr->nextPointer = newNode; must be the very last line.");
    task->layout = "1  newNode->prevPointer = curNodePtr;\n"
                   "2  newNode->nextPointer = ...;\n"
                   "3  curNodePtr->nextPointer->prevPointer = ...;\n"
                   "4  curNodePtr->nextPointer = ...;";

    task = startTask(level, 1, TASK_TYPE, "Type the last link: curNodePtr points forward at the new node.",
                     "// curNodePtr is the node just before the position\n"
                     "newNode->prevPointer = curNodePtr;\n"
                     "newNode->nextPointer = curNodePtr->nextPointer;\n"
                     "if (curNodePtr->nextPointer != nullptr) {\n"
                     "    curNodePtr->nextPointer->prevPointer = newNode;\n"
                     "}\n"
                     "____");
    addAnswer(task, "curNodePtr->nextPointer = newNode;");
    addMistake(task, "newNode->nextPointer = curNodePtr;", "That's a link on newNode. The one left to change is curNodePtr's forward link.");
    addMistake(task, "curNodePtr->prevPointer = newNode;", "newNode comes AFTER curNodePtr, so it's the forward link.");
    addMistake(task, "curNodePtr = newNode;", "That only moves your pointer. A link has to change.");
    setHints(task, "Only one link is left: curNodePtr's forward link.",
                   "It should lead to the new node.",
                   "curNodePtr->nextPointer = ...;");

    // =========================================================
    // Chapter 8: Lab 1 review (Coding Exam 1). [ ] is allowed here.
    // =========================================================

    // ---------- Level 47 ----------
    level = startLevel(47, 8, "frequencyCount", "freqCount", "Lab 1 Q13");
    level->explanation = "Loop over an array with an index i from 0 up to size - 1 and use arr[i] to read element i. To count "
                         "how many elements equal a value, add 1 to count each time one matches. (Lab 1 allows [ ].)";
    level->example = "// counting how many are bigger than 10\n"
                     "int count = 0;\n"
                     "for (int i = 0; i < size; i++) {\n"
                     "    if (arr[i] > 10) count++;\n"
                     "}";
    level->inGame = "The game counts how many of your group are left with frequencyCount.";
    level->slideTitle = "Lab 1, Q13";
    level->slideText = "// Q13: Count the frequency of a given element in an array.\n"
                       "int frequencyCount(int size, int arr[], int value) {\n"
                       "    return 0;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Type the line inside the loop that adds 1 to count when arr[i] equals value.",
                     "int frequencyCount(int size, int arr[], int value) {\n"
                     "    int count = 0;\n"
                     "    for (int i = 0; i < size; i++) {\n"
                     "        ____\n"
                     "    }\n"
                     "    return count;\n"
                     "}");
    addAnswer(task, "if (arr[i] == value) count++;");
    addAnswer(task, "if (arr[i] == value) { count++; }");
    addAnswer(task, "if (arr[i] == value) count = count + 1;");
    addAnswer(task, "if (arr[i] == value) count += 1;");
    addAnswer(task, "if (value == arr[i]) count++;");
    addMistake(task, "if (arr[i] = value) count++;", "A single = sets arr[i]. To compare, use ==.");
    addMistake(task, "if (arr == value) count++;", "Compare one element, not the whole array.");
    addMistake(task, "if (arr[i] == value) count--;", "count-- takes 1 away.");
    setHints(task, "Check whether element i matches value.",
                   "If it does, count goes up by 1.",
                   "if (...) count++;");

    task = startTask(level, 1, TASK_TYPE, "Type the for line that loops i from 0 up to (but not including) size.",
                     "int count = 0;\n"
                     "____\n"
                     "    if (arr[i] == value) count++;\n"
                     "}");
    addAnswer(task, "for (int i = 0; i < size; i++) {");
    addAnswer(task, "for (int i = 0; i < size; ++i) {");
    addMistake(task, "for (int i = 0; i <= size; i++) {", "<= size goes one past the end: the last element is at size - 1.");
    addMistake(task, "for (int i = 1; i < size; i++) {", "Arrays start at 0, so this skips the first element.");
    addMistake(task, "for (i = 0; i < size; i++) {", "i hasn't been declared yet.");
    setHints(task, "Start i at the first index.",
                   "Stop before i reaches size.",
                   "End the line with { because the } is already there.");

    // ---------- Level 48 ----------
    level = startLevel(48, 8, "countDuplicates", "dupes", "Lab 1 Q14");
    level->explanation = "To compare every pair of elements, use two loops. The inner loop starts at j = i + 1, so each pair is "
                         "checked once and an element is never compared with itself.";
    level->example = "for (int i = 0; i < size; i++) {\n"
                     "    for (int j = i + 1; j < size; j++) {\n"
                     "        // compare arr[i] with arr[j]\n"
                     "    }\n"
                     "}";
    level->inGame = "Before every game, countDuplicates checks the rack. The result is the first line of the shot log.";
    level->slideTitle = "Lab 1, Q14";
    level->slideText = "// Q14: Count the total number of duplicate elements in an array.\n"
                       "int countDuplicates(int size, int arr[]) {\n"
                       "    return 0;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Type the if line that adds 1 to count when arr[i] and arr[j] match.",
                     "for (int i = 0; i < size; i++) {\n"
                     "    for (int j = i + 1; j < size; j++) {\n"
                     "        ____\n"
                     "    }\n"
                     "}");
    addAnswer(task, "if (arr[i] == arr[j]) count++;");
    addAnswer(task, "if (arr[i] == arr[j]) { count++; }");
    addAnswer(task, "if (arr[j] == arr[i]) count++;");
    addAnswer(task, "if (arr[i] == arr[j]) count += 1;");
    addMistake(task, "if (arr[i] == arr[i]) count++;", "That compares an element with itself, so it's always true.");
    addMistake(task, "if (arr[i] = arr[j]) count++;", "A single = copies. To compare, use ==.");
    setHints(task, "Compare the two elements the loops point at.",
                   "They're arr[i] and arr[j].",
                   "if (...) count++;");

    task = startTask(level, 1, TASK_TYPE, "Type the inner for line so that j starts just after i.",
                     "int countDuplicates(int size, int arr[]) {\n"
                     "    int count = 0;\n"
                     "    for (int i = 0; i < size; i++) {\n"
                     "        ____\n"
                     "            if (arr[i] == arr[j]) count++;\n"
                     "        }\n"
                     "    }\n"
                     "    return count;\n"
                     "}");
    addAnswer(task, "for (int j = i + 1; j < size; j++) {");
    addAnswer(task, "for (int j = i + 1; j < size; ++j) {");
    addAnswer(task, "for (int j = 1 + i; j < size; j++) {");
    addMistake(task, "for (int j = 0; j < size; j++) {", "Starting j at 0 compares each element with itself, and checks every pair twice.");
    addMistake(task, "for (int j = i; j < size; j++) {", "j = i compares arr[i] with itself.");
    addMistake(task, "for (int i = i + 1; i < size; i++) {", "The inner loop needs its own variable.");
    setHints(task, "The inner loop uses its own variable, j.",
                   "j should start one place after i.",
                   "for (int j = ...; j < size; j++) {");

    // ---------- Level 49 ----------
    level = startLevel(49, 8, "reverse", "reverse", "Lab 1 Q15");
    level->explanation = "To reverse an array in place, swap the first and last elements, then the second and second-last, and so "
                         "on, stopping at the middle. The partner of element i is element size - 1 - i. swap_ints can do the "
                         "swapping if you give it the two addresses.";
    level->example = "// size 5: swap 0 with 4, then 1 with 3 (2 stays put)";
    level->inGame = "Not used by the game - reverse_array (Level 17) does the real work.";
    level->slideTitle = "Lab 1, Q15";
    level->slideText = "// Q15: Reverse an array in place using only 1 array.\n"
                       "void reverse(int size, int arr[]) {\n"
                       "    return;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Type the line inside the loop that swaps element i with its partner at the other end, using swap_ints.",
                     "void reverse(int size, int arr[]) {\n"
                     "    for (int i = 0; i < size / 2; i++) {\n"
                     "        ____\n"
                     "    }\n"
                     "}");
    addAnswer(task, "swap_ints(&arr[i], &arr[size - 1 - i]);");
    addAnswer(task, "swap_ints(&arr[i], &arr[size - i - 1]);");
    addAnswer(task, "swap_ints(&arr[size - 1 - i], &arr[i]);");
    addAnswer(task, "swap_ints(&arr[size - i - 1], &arr[i]);");
    addAnswer(task, "int temp = arr[i]; arr[i] = arr[size - 1 - i]; arr[size - 1 - i] = temp;");
    addAnswer(task, "int temp = arr[i]; arr[i] = arr[size - i - 1]; arr[size - i - 1] = temp;");
    addMistake(task, "swap_ints(&arr[i], &arr[size - i]);", "When i is 0, size - i is one past the end of the array.");
    addMistake(task, "swap_ints(arr[i], arr[size - 1 - i]);", "swap_ints takes addresses, not values.");
    addMistake(task, "swap_ints(&arr[i], &arr[size - 1]);", "That always swaps with the last element. The partner should move too.");
    setHints(task, "Element 0's partner is the last element, size - 1.",
                   "As i goes up by 1, the partner goes down by 1.",
                   "swap_ints needs the address of each element.");

    task = startTask(level, 1, TASK_TYPE, "Type the for line so the loop stops at the middle of the array.",
                     "void reverse(int size, int arr[]) {\n"
                     "    ____\n"
                     "        swap_ints(&arr[i], &arr[size - 1 - i]);\n"
                     "    }\n"
                     "}");
    addAnswer(task, "for (int i = 0; i < size / 2; i++) {");
    addAnswer(task, "for (int i = 0; i < size / 2; ++i) {");
    addMistake(task, "for (int i = 0; i < size; i++) {", "Going all the way swaps every pair twice, which puts the array back how it was.");
    addMistake(task, "for (int i = 0; i <= size / 2; i++) {", "<= goes one step too far when size is even.");
    setHints(task, "Each swap fixes two elements.",
                   "So you only need to go halfway.",
                   "Stop before i reaches size / 2.");

    // ---------- Level 50 ----------
    level = startLevel(50, 8, "rotateLeft", "rotateLeft", "Lab 1 Q16");
    level->explanation = "To rotate left by one, save the first element, move every other element one place to the left, then put "
                         "the saved one at the end: {1, 2, 3, 4} becomes {2, 3, 4, 1}.";
    level->example = "int first = arr[0];\n"
                     "// ... shift everything one place left ...\n"
                     "// ... then put first at the end";
    level->inGame = "Not used by the game - it's Coding Exam 1 practice.";
    level->slideTitle = "Lab 1, Q16";
    level->slideText = "// Q16: Left rotate an array by one position, moving the first element to the end.\n"
                       "int rotateLeft(int size, int arr[]) {\n"
                       "    return -1;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Type the line inside the loop that moves each element one place to the left.",
                     "int first = arr[0];\n"
                     "for (int i = 0; i < size - 1; i++) {\n"
                     "    ____\n"
                     "}\n"
                     "arr[size - 1] = first;");
    addAnswer(task, "arr[i] = arr[i + 1];");
    addAnswer(task, "arr[i] = arr[1 + i];");
    addMistake(task, "arr[i + 1] = arr[i];", "That copies to the right.");
    addMistake(task, "arr[i] = arr[i - 1];", "arr[i - 1] is the element before. Moving left means taking from the one after.");
    addMistake(task, "arr[i] = arr[i];", "That copies an element onto itself.");
    setHints(task, "Each place takes the value of its neighbour.",
                   "Left shift: the value comes from the right.",
                   "arr[i] = ...;");

    task = startTask(level, 1, TASK_TYPE, "Type the last line, which puts the saved first element at the end.",
                     "int first = arr[0];\n"
                     "for (int i = 0; i < size - 1; i++) {\n"
                     "    arr[i] = arr[i + 1];\n"
                     "}\n"
                     "____");
    addAnswer(task, "arr[size - 1] = first;");
    addMistake(task, "arr[size] = first;", "arr[size] is past the end of the array.");
    addMistake(task, "first = arr[size - 1];", "That's the wrong direction: the last element should get first.");
    addMistake(task, "arr[0] = first;", "first goes at the end, not back at the start.");
    setHints(task, "Where does the saved element go?",
                   "The last element of the array.",
                   "Its index is size - 1.");

    // ---------- Level 51 ----------
    level = startLevel(51, 8, "twoMovies", "twoMovies", "Lab 1 Q17");
    level->explanation = "The same two loops can find two different elements that add up to a target. As soon as you find a pair, "
                         "return true. If the loops finish without finding one, return false.";
    level->example = "for (int i = 0; i < size; i++)\n"
                     "    for (int j = i + 1; j < size; j++)\n"
                     "        // is arr[i] + arr[j] the target?";
    level->inGame = "Not used by the game - it's Coding Exam 1 practice.";
    level->slideTitle = "Lab 1, Q17";
    level->slideText = "// Q17: Given a flight length and an array of movie lengths, return true if\n"
                       "// there are exactly two movies whose runtimes sum to the flight length.\n"
                       "// Users watch exactly two movies and cannot watch the same movie twice.\n"
                       "bool twoMovies(int flightLength, int movieLengths[], int size) {\n"
                       "    return false;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Type the if line that returns true when movie i and movie j add up to flightLength.",
                     "bool twoMovies(int flightLength, int movieLengths[], int size) {\n"
                     "    for (int i = 0; i < size; i++) {\n"
                     "        for (int j = i + 1; j < size; j++) {\n"
                     "            ____\n"
                     "        }\n"
                     "    }\n"
                     "    return false;\n"
                     "}");
    addAnswer(task, "if (movieLengths[i] + movieLengths[j] == flightLength) return true;");
    addAnswer(task, "if (movieLengths[i] + movieLengths[j] == flightLength) { return true; }");
    addAnswer(task, "if (movieLengths[j] + movieLengths[i] == flightLength) return true;");
    addAnswer(task, "if (flightLength == movieLengths[i] + movieLengths[j]) return true;");
    addMistake(task, "if (movieLengths[i] + movieLengths[i] == flightLength) return true;", "That's the same movie twice.");
    addMistake(task, "if (movieLengths[i] + movieLengths[j] = flightLength) return true;", "A single = assigns. To compare, use ==.");
    addMistake(task, "if (movieLengths[i] + movieLengths[j] == flightLength) return false;", "Finding a pair means the answer is yes.");
    setHints(task, "Add the two movies the loops are looking at.",
                   "Compare the total with flightLength.",
                   "If they match, return true.");

    task = startTask(level, 1, TASK_TRACE, "What does this call return? Type true or false.",
                     "int movies[] = {90, 60, 30, 45};\n"
                     "bool ok = twoMovies(120, movies, 4);");
    addAnswer(task, "true");
    addAnswer(task, "True");
    addMistake(task, "false", "Check every pair: is there one that adds up to exactly 120?");
    addMistake(task, "False", "Check every pair: is there one that adds up to exactly 120?");
    setHints(task, "Try every pair of different movies.",
                   "90 + 60? 90 + 30? ...",
                   "Is any total exactly 120?");
    task->layout = "Pairs:  90+60 = 150   90+30 = ?   90+45 = ?\n"
                   "        60+30 = ?    60+45 = ?   30+45 = ?\n"
                   "Is any pair exactly 120?";

    // ---------- Level 52 ----------
    level = startLevel(52, 8, "wordCounter", "wordCount", "Lab 1 Q18");
    level->explanation = "A word is one or more letters. Walk along the chars and remember whether you're inside a word. A new word "
                         "starts when you find a letter and you weren't already in a word: that's when to count.";
    level->example = "char s[] = \"hi there\";\n"
                     "// words start at the 'h' and the 't',\n"
                     "// so there are 2 words";
    level->inGame = "Not used by the game - it's Coding Exam 1 practice.";
    level->slideTitle = "Lab 1, Q18";
    level->slideText = "// Q18: Count the number of separate words in an array of chars, where a word\n"
                       "// is one or more characters in ranges A..Z or a..z separated by spaces.\n"
                       "int wordCounter(int size, char characters[]) {\n"
                       "    return 0;\n"
                       "}";

    task = startTask(level, 0, TASK_TYPE, "Type the if line that adds 1 to count when a letter starts a new word.",
                     "int wordCounter(int size, char characters[]) {\n"
                     "    int count = 0;\n"
                     "    bool inWord = false;\n"
                     "    for (int i = 0; i < size; i++) {\n"
                     "        char c = characters[i];\n"
                     "        bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');\n"
                     "        ____\n"
                     "        inWord = isLetter;\n"
                     "    }\n"
                     "    return count;\n"
                     "}");
    addAnswer(task, "if (isLetter && !inWord) count++;");
    addAnswer(task, "if (!inWord && isLetter) count++;");
    addAnswer(task, "if (isLetter && inWord == false) count++;");
    addAnswer(task, "if (isLetter && !inWord) { count++; }");
    addAnswer(task, "if (isLetter == true && inWord == false) count++;");
    addMistake(task, "if (isLetter) count++;", "That counts every letter, not every word.");
    addMistake(task, "if (!isLetter && inWord) count++;", "That counts where words end, so a word at the very end is missed.");
    addMistake(task, "if (isLetter || !inWord) count++;", "|| is true if just one of them is. Both need to be true.");
    setHints(task, "A word starts at a letter...",
                   "...that comes when you weren't already in a word.",
                   "Both conditions together, then count++;");

    task = startTask(level, 1, TASK_TYPE, "Type the line that works out whether c is a letter (A to Z, or a to z).",
                     "for (int i = 0; i < size; i++) {\n"
                     "    char c = characters[i];\n"
                     "    ____\n"
                     "    if (isLetter && !inWord) count++;\n"
                     "    inWord = isLetter;\n"
                     "}");
    addAnswer(task, "bool isLetter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');");
    addAnswer(task, "bool isLetter = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');");
    addAnswer(task, "bool isLetter = c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z';");
    addAnswer(task, "bool isLetter = c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z';");
    addMistake(task, "bool isLetter = (c >= 'A' || c <= 'Z') || (c >= 'a' || c <= 'z');", "c >= 'A' || c <= 'Z' is true for every char. Inside each bracket both parts must be true.");
    addMistake(task, "bool isLetter = (c >= \"A\" && c <= \"Z\") || (c >= \"a\" && c <= \"z\");", "Double quotes make strings. One character needs single quotes.");
    setHints(task, "A capital letter is between 'A' and 'Z'.",
                   "A small letter is between 'a' and 'z'.",
                   "Either one counts, so join the two checks with ||.");

    // ---------- Level 53 ----------
    level = startLevel(53, 8, "PrintArray (2D)", "PrintArray", "Lab 1 Q19");
    level->explanation = "A 2D array is indexed with two numbers: array[row][col]. To print it in rows and columns, loop over the "
                         "rows, and inside that loop over the columns. Lab 1 says PrintArray is the only function allowed to use "
                         "printf.";
    level->example = "int grid[2][3] = { {1, 2, 3},\n"
                     "                   {4, 5, 6} };\n"
                     "// grid[1][0] is 4: row 1, column 0";
    level->inGame = "Not used by the game - it's Coding Exam 1 practice.";
    level->slideTitle = "Lab 1, Q19";
    level->slideText = "// Q19: PrintArray\n"
                       "// Prints the 4x6 array in rows and columns.\n"
                       "// This is the ONLY function allowed to use printf.\n"
                       "void PrintArray(int array[4][6])\n"
                       "{\n"
                       "}\n"
                       "\n"
                       "From the Lab 1 main:\n"
                       "int array[4][6] = { {0,0,3,1,3,4},\n"
                       "                    {0,0,2,3,4,3},\n"
                       "                    {0,0,1,3,3,2},\n"
                       "                    {0,0,1,1,1,1} };";

    task = startTask(level, 0, TASK_TYPE, "Type the line that prints one element, followed by a space.",
                     "void PrintArray(int array[4][6]) {\n"
                     "    for (int row = 0; row < 4; row++) {\n"
                     "        for (int col = 0; col < 6; col++) {\n"
                     "            ____\n"
                     "        }\n"
                     "        printf(\"\\n\");\n"
                     "    }\n"
                     "}");
    addAnswer(task, "printf(\"%d \", array[row][col]);");
    addAnswer(task, "printf(\"%d\\t\", array[row][col]);");
    addAnswer(task, "printf(\"%3d\", array[row][col]);");
    addMistake(task, "printf(\"%d \", array[col][row]);", "The first index picks the row and the second the column.");
    addMistake(task, "printf(\"%d \", array);", "That passes the whole array. Print one element.");
    addMistake(task, "cout << array[row][col] << \" \";", "Lab 1 says PrintArray must use printf.");
    setHints(task, "printf needs a format for an int.",
                   "The element is at [row][col].",
                   "printf(\"%d \", ...);");

    task = startTask(level, 1, TASK_TYPE, "Type the inner for line that goes along the 6 columns.",
                     "void PrintArray(int array[4][6]) {\n"
                     "    for (int row = 0; row < 4; row++) {\n"
                     "        ____\n"
                     "            printf(\"%d \", array[row][col]);\n"
                     "        }\n"
                     "        printf(\"\\n\");\n"
                     "    }\n"
                     "}");
    addAnswer(task, "for (int col = 0; col < 6; col++) {");
    addAnswer(task, "for (int col = 0; col < 6; ++col) {");
    addMistake(task, "for (int col = 0; col <= 6; col++) {", "<= 6 goes one past the last column (index 5).");
    addMistake(task, "for (int col = 0; col < 4; col++) {", "4 is the number of rows. Each row has 6 columns.");
    setHints(task, "The array is 4 rows by 6 columns.",
                   "Columns go from 0 to 5.",
                   "End the line with { because the } is already there.");

    // ---------- Level 54 ----------
    level = startLevel(54, 8, "Grid clusters", "clusters", "Lab 1 Q20-Q21 (the lab only describes these, so this is one correct way)");
    level->explanation = "Q20 looks for 3 identical items in a row, checking horizontally first. Moving along a row means the "
                         "column changes and the row stays the same. Q21 makes items fall down a column into the gaps (the -1's), "
                         "and new items at the top get -2.";
    level->example = "// right of (row, col) is (row, col + 1)\n"
                     "// below (row, col) is (row + 1, col)";
    level->inGame = "Not used by the game - it's Coding Exam 1 practice.";
    level->slideTitle = "Lab 1, Q20-Q21";
    level->slideText = "// Q20: ReplaceWithMinusOne\n"
                       "// Scans the array from position (0,0), moving left-to-right across each row.\n"
                       "// At each position, checks for a HORIZONTAL line of 3+ identical items first,\n"
                       "// and if none found, checks for a VERTICAL line of 3+ identical items.\n"
                       "// The first cluster found is scored by summing its values (1-4), then the\n"
                       "// cluster is replaced with -1's. 0's and -2's are never part of a cluster.\n"
                       "\n"
                       "// Q21: FallDownAndReplace\n"
                       "// Removes the -1's by making all items above them \"fall down\" into the empty\n"
                       "// space. New items added at the top of the array are given the value -2, and\n"
                       "// these will not be part of the next cluster. Works column by column.";

    task = startTask(level, 0, TASK_TYPE, "Q20: fill in the check for a horizontal line of 3 the same, starting at (row, col).",
                     "// 0's, -1's and -2's are never part of a cluster\n"
                     "int v = array[row][col];\n"
                     "if (v > 0 && col + 2 < 6 && ____) {\n"
                     "    // found a horizontal cluster of 3\n"
                     "}");
    addAnswer(task, "array[row][col + 1] == v && array[row][col + 2] == v");
    addAnswer(task, "array[row][col + 2] == v && array[row][col + 1] == v");
    addAnswer(task, "v == array[row][col + 1] && v == array[row][col + 2]");
    addMistake(task, "array[row + 1][col] == v && array[row + 2][col] == v", "That checks downwards (a vertical line). Q20 checks along the row first.");
    addMistake(task, "array[row][col + 1] == v || array[row][col + 2] == v", "|| is true if just one matches. Both need to.");
    setHints(task, "Horizontal means along the row: col changes, row stays.",
                   "The next two items are at col + 1 and col + 2.",
                   "Both must equal v.");

    task = startTask(level, 1, TASK_TYPE, "Q21: the item at (row, col) was removed. Type the line that makes each item above it fall down one place.",
                     "for (int r = row; r > 0; r--) {\n"
                     "    ____\n"
                     "}\n"
                     "array[0][col] = -2;   // a new item appears at the top");
    addAnswer(task, "array[r][col] = array[r - 1][col];");
    addMistake(task, "array[r - 1][col] = array[r][col];", "That copies upwards. Each place should take the value from the one above it.");
    addMistake(task, "array[r][col] = array[r + 1][col];", "r + 1 is the row below. Falling down means taking the value from above.");
    addMistake(task, "array[r][col] = array[r][col - 1];", "col - 1 is the item to the left. Items fall within the same column.");
    setHints(task, "Work up the column from the gap.",
                   "Each place takes the value of the place above it.",
                   "The place above is in row r - 1.");
}
