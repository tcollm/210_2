#include "types.h"

extern struct NODE *root;
extern struct NODE *cwd;

struct NODE *findNode(struct NODE *current, const char *target)
{
    if (current == NULL)
    {
        return NULL;
    }

    // target is found
    if (strcmp(current->name, target) == 0)
    {
        return current;
    }

    // recursively search children
    struct NODE *foundInChild = findNode(current->childPtr, target);
    if (foundInChild != NULL)
    {
        return foundInChild;
    }

    // recursively search children
    struct NODE *foundInSibling = findNode(current->siblingPtr, target);
    if (foundInSibling != NULL)
    {
        return foundInSibling;
    }

    // not found
    return NULL;
}

struct NODE *splitPath(char *pathName, char *baseName, char *dirName)
{
    int i = 0;
    int lastIndex = -1;

    while (pathName[i] != '\0')
    {
        // find the last / so that dirName can be everything before it
        // and baseName can be the dir after it
        if (pathName[i] == '/')
        {
            lastIndex = i;
            // return; DO NOT RETURN - will cause the loop to not work
        }
        i++;
    }

    // assign baseName and dirName
    // if there are no '/' then assign dirName to the root
    // and assign the pathName to baseName
    if (lastIndex == -1)
    {
        dirName[0] = '/';
        strcpy(baseName, pathName);
    }
    else
    {
        strncpy(dirName, pathName, lastIndex);
        strcpy(baseName, pathName + lastIndex + 1);
    }
    dirName[lastIndex] = '\0'; // Null-terminate dirName

    // start at root if path starts with '/'
    struct NODE *start = (dirName[0] == '/') ? root : cwd;

    // find the dir node using its name
    struct NODE *dirNode = findNode(start, dirName);

    // return a node
    if (dirNode && dirNode->fileType == 'D')
    {
        return dirNode;
    }
    else
    {
        // if dirName is not a directory then return ERROR
        printf("ERROR: '%s' is not a valid directory path\n", dirName);
        free(baseName);
        free(dirName);
        return NULL;
    }
}

void createNode(char *baseName, struct NODE *parentDir)
{
    struct NODE *newNode = (struct NODE *)malloc(sizeof(struct NODE));

    if (!newNode)
    {
        printf("ERROR: Memory allocation failed\n");
        return;
    }

    strcpy(newNode->name, baseName);
    newNode->fileType = 'D';
    newNode->childPtr = NULL;
    newNode->siblingPtr = NULL;
    newNode->parentPtr = parentDir;

    // attach to parent
    struct NODE *child = parentDir->childPtr;

    // parent's child is empty
    if (child == NULL)
    {
        parentDir->childPtr = newNode;
    }
    else
    {
        // parent's child is not empty, search for next empty sibling
        while (child->siblingPtr != NULL)
        {
            child = child->siblingPtr;
        }
        child->siblingPtr = newNode;
    }
}

void mkdir(char pathName[])
{
    printf("INITIAL TREE: \n");
    printTree();

    char *baseName = (char *)malloc(sizeof(char) * 256);
    char *dirName = (char *)malloc(sizeof(char) * 256);

    // splitPath(pathName, baseName, dirName);
    // struct NODE *dirNode = findNode(parentDir, dirName);

    struct NODE *dirNode = splitPath(pathName, baseName, dirName);

    if (dirNode == NULL)
    {
        printf("ERROR: Parent directory '%s' does not exist\n", dirName);
        free(baseName);
        free(dirName);
        return;
    }

    // Check if baseName already exists in the parentDir
    if (findNode(dirNode, baseName) != NULL)
    {
        printf("ERROR: '%s' already exists in directory '%s'\n", baseName, dirName);
        free(baseName);
        free(dirName);
        return;
    }

    createNode(baseName, dirNode); // Create the new directory node

    printf("MKDIR SUCCESS: node '%s' successfully created\n", pathName);
    printf("FINAL TREE: \n");
    printTree();

    free(baseName);
    free(dirName);
}
