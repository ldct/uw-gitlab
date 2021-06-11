#include <stdlib.h>
#include <stdbool.h>
#include <map.h>
#include <bwio.h>


int max(int a, int b){
    return a>b? a: b;
}

void initializeNode(Node* node, int key, void* value){
    node->key = key;
    node->value = value;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    node->up = NULL;
}

void freeNode(Map* map, Node* node){
    push(&(map->freeQueue), node);
}

void initializeMap(Map* map){
    map->root = NULL;
    initializeQueue("generic map", &(map->freeQueue));
    int i=0;
    for(i=0;i<QUEUE_SIZE;i++){
        push(&(map->freeQueue), map->nodes + i);
    }
}

void updateHeight(Node* node){
    if(!node)
        return;
    int lh = 0, rh = 0;
    if (node->left) {
        node->left->up = node;
        lh = node->left->height;
    }
    if (node->right) {
        node->right->up = node;
        rh = node->right->height;
    }
    node->height = max(lh, rh) + 1;
}

Node* rotation(Node* position){
    if(!position){
        return position;
    }
    int lh = position->left ? position->left->height : 0;
    int rh = position->right? position->right->height : 0;
    int bal = lh - rh;
    if(bal>1){
        //zig zag left
        //      o top
        //  o outlier
        //      o root
        int lrh = position->left->right? position->left->right->height : 0;
        int llh = position->left->left? position->left->left->height : 0;
        if(lrh > llh){
            Node* outlier = position->left;
            Node* root = outlier->right;
            outlier->right = root->left;
            position->left = root->right;
            root->left = outlier;
            root->right = position;
            updateHeight(outlier);
            updateHeight(position);
            updateHeight(root);
            return root;
        } else {//straight line
            Node* median  = position->left;
            position->left = median->right;
            median->right = position;
            updateHeight(position);
            updateHeight(median);
            return median;
        }
    } else if(bal<-1){
        int rlh = position->right->left? position->right->left->height : 0;
        int rrh = position->right->right? position->right->right->height : 0;
        if(rlh > rrh){
            Node* outlier = position->right;
            Node* root = outlier->left;
            outlier->left = root->right;
            position->right = root->left;
            root->right = outlier;
            root->left = position;
            updateHeight(outlier);
            updateHeight(position);
            updateHeight(root);
            return root;

        } else {
            Node* median  = position->right;
            position->right = median->left;
            median->left = position;
            updateHeight(position);
            updateHeight(median);
            return median;
        }
    }
    updateHeight(position);
    return position;
}


Node* insertNode(Node* position, Node* node){
    if(!position)
        return node;
    if(position->key>node->key){
        position->left = insertNode(position->left, node);
    } else {
        position->right = insertNode(position->right, node);
    }
    return rotation(position);
}

Node* putNode(Map* map, Node* position, int key, void* value){
    if(!position){
        Node* node = pop(&(map->freeQueue));
        if(!node){
            map->retainer = NULL;
            return node;
        }
        map->retainer = value;
        initializeNode(node, key, value);
        return node;
    }
    if(position->key > key){
        position->left = putNode(map, position->left, key, value);
    } else if(position->key < key){
        position->right = putNode(map, position->right, key, value);
    } else {
        map->retainer = position->value;
        position->value = value;
    }
    return rotation(position);
}

Node* promote(Node* position, bool left){
    if(!position)
        return NULL;
    Node* chosen = NULL;
    if(left){
        if(position->left){
            if(position->left->left){
                chosen = promote(position->left, left);
            } else {
                //end of the line, the chosen one
                chosen = position->left;
                position->left = chosen->right;
            }
        }
    } else {
        if(position->right){
            if(position->right->right){
                chosen = promote(position->right, left);
            } else {
                //end of the line, the chosen one
                chosen = position->right;
                position->right = chosen->left;
            }
        }
    }
    updateHeight(position);
    return chosen;
}

Node* successor(Map* map, Node* position){
    Node* candidate = NULL;
    if(position->right){
        if(!position->right->left){
            candidate = position->right;
            candidate->left = position->left;
        } else {
            candidate = promote(position->right, true);
            candidate->left = position->left;
            candidate->right = position->right;
        }
    } else if(position->left){
        if(!position->left->right){
            candidate = position->left;
            candidate->right = position->right;
        } else {
            candidate = promote(position->left, false);
            candidate->left = position->left;
            candidate->right = position->right;
        }
    }
    map->retainer = position->value;
    freeNode(map, position);
    updateHeight(candidate);
    return candidate;
}

Node* removeNode(Map* map, Node* position, int key){
    if(!position)
        return NULL;
    if(position->key>key){
        position->left = removeNode(map, position->left, key);

    } else if(position->key == key) {
        position = successor(map, position);
    } else {
        position->right = removeNode(map, position->right, key);
    }
    updateHeight(position);
    return rotation(position);
}

void* search(const Node* node, int key){
    if(!node)
        return NULL;
    if(node->key>key){
        return search(node->left, key);
    } else if (node->key==key) {
        return node->value;
    } else{
        return search(node->right, key);
    }
}

Node* iterateNode(Node* node){
    Node* up;
    if (node->right) {
        //if the right actually has content, always start by returning the left most of the branch
        up = node->right;
        while(up->left)
            up = up->left;
        return up;
    }
    while ((up=node->up)) {
        if(up->left == node) {
            bwprintf(COM2, "Left\r\n");
            return up;
        }
        else {
        node = up;
        }
    }
    if(!up){
    }
    return up;
}

int insertMap(Map* map, int key, void* value){
    Node* node = pop(&(map->freeQueue));
    if(!node)
        return -1;
    initializeNode(node, key, value);
    map->root = insertNode(map->root, node);
    return 0;
}

//Dynamo Style put operation
int putMap(Map* map, int key, void* value){
    map->retainer = NULL;
    map->root = putNode(map, map->root, key, value);
    if (map->root) {
        map->root->up = NULL;
    }
    if(map->retainer == value){
        //newly created, usually
        return 1;
    } else if (map->retainer){
        //updated, the most common case
        return 0;
    } else {
        //ran out of resources for insertion
        return -1;
    }
}


void* getMap(const Map* map, int key){
    return search(map->root, key);
}

void* removeMap(Map* map, int key) {
    map->retainer = NULL;
    map->root = removeNode(map, map->root, key);
    if (map->root) {
        map->root->up = NULL;
    }
    return map->retainer;
}

Node* iterateMap(Map* map, Node* key){
    if (!map->root) {
        return NULL;
    }
    if (!key) {
        key = map->root;
        while(key->left)
            key = key->left;
        return key;
    }
    return iterateNode(key);
}

void debugTree(Node* node){
    if(!node) return;
    bwprintf(COM2, " %d " ,node->key);
    debugTree(node->left);
    debugTree(node->right);
}
void printTree(Map* map){
    bwprintf(COM2,"Printing trees\r\n");
    debugTree(map->root);
    bwprintf(COM2,"Doneish\r\n");
}
