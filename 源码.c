#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*用Huffman算法写出一个程序实现文件的压缩(和解压缩)*/
//怎么输入数据，这里牵扯到字母转ASCII二进制
//输出二进制

//大体流程
//将读取的数据放进一个数组
//这个数组的元素是指针，指向dumb域, name域, frequency域等域的结构
//在把这个数组变成二叉堆

#define characterKind 128       //7位可以代表128个字符
typedef struct character *leaf; //这个用于之后建树8/8
struct character
{
    int dumb; //判断是否为哑结点
    char name;
    int frequency;
    leaf left;  //左树叶
    leaf right; //右树叶
};

//栈的例程
typedef struct stackNode *Stack;
typedef struct stackNode *Postion;
typedef struct stackNode *SNode;
struct stackNode
{
    int element;
    Postion next;
};

int pop(Stack S);
void push(int element, Stack S);
int isEmpty(Stack S);

//将数据收集起来，设字符种类为M，个数为N。该例程运行时间为O(N*M)
void collectData(char filename[], struct character *collectTable[], int *size);

//开始建堆
typedef struct character *queueNode;      //插入到堆的元素
typedef struct character **priorityQueue; //相当于一个指向指针的指针，即一个数组
typedef struct character *queueMin;       //指向堆中最小的元素的指针
//这个堆需要buildHeap(), deleteMin(),downFilter(), insert()等例程
priorityQueue buildHeap(struct character *collectTable[], int *size);
queueMin deleteMin(priorityQueue Q, int *size);
void downFiler(int i, struct character *collectTable[], int *size); //将collectTable[i]指向的元素下滤到正确位置
void insert(queueNode q, priorityQueue Q, int *size);

//进行两次deleteMin()并合并MIN，再把合并的元素放进去,直到deleteMin()为空
//其实最多执行N-1次合并就结束了,但我们以collectTableSize作为参照
typedef struct character *dumb; //哑结点，连接两个结点的结点(或是连接一个或者两个树叶的结点)
//要建立一个编码表，来记录生成的huffman编码

//用散列表，设置一个128个元素的数组(可以装下ASCII码表的所有元素)
//该数组每一个元素是一个指针，指向一个整型变量encoding
typedef char *HashTable;
//建立散列表的例程

enum direction
{
    leftTree,
    rightTree
}; //用于判断是左树叶还是右树叶,左树叶是编码为0，右树叶编码是1
//1.构建huffman树.
//将它们的和作为哈夫曼树的权值节点,构建到huffman树中;
typedef struct character *huffmanTree;
huffmanTree buildHuffmanTree(priorityQueue Q, huffmanTree T, int *size);
//2.通过哈夫曼树产生哈夫曼编码;
//规则是：从根节点出发，往左走-->0 ，往右走-->1,
//遇到叶子节点的情况，就将它对应的huffman编码写入数组中。
void buildCodingSchedule(huffmanTree T, HashTable codingSchedule[], int bits, int *size, const int tableSize);
void encoding(huffmanTree T, char bitCoding[], char *codingSchedule[], int bits, enum direction dire, const int tableSize); //该例程遍历Huffman树，遍历到非哑结点时将数据记录到散列表

//我们在解压之前要编写配置文件
//我们要解压的话必须要知道这可huffman树，所以在压缩的时候需要编写一个配置文件来
//存储huffman树的信息（各个字符以及字符出现的次数）。
//在配置文件里面将：字符+字符出现的次数存在一行
//在这里要使用itoa这个函数，将次数转换成一个字符串(string)类型存储
//压缩与解压例程
char *compress(char *filename, HashTable codingSchedule[], int *compressNum);                         //返回压缩的字符个数
void WriteToTheConfiguration(priorityQueue Q, int collectTableSizeCopy, char *configurationFileName); //写入配置文件的例程
priorityQueue ReadConfigurationFile(char *ConfigurationFile, int *sum, int *collectTableSize);        //返回一个二叉堆，为解码的时候排除附加的08

char *decompression(char *filename, huffmanTree T, int *sum);

void Compress();
void Decompress();

int main()
{
    int selection = 3; //0代表选择压缩，1代表解压; 初始是3为了进while循环
    //如果输入字符给selection，程序会自动停止
    while ((selection == 0 || selection == 1) || selection != 2)
    {
        if (selection == 0)
            Compress();
        else if (selection == 1)
            Decompress();

        printf("\n请选择服务：0.压缩    1.解压\n");
        printf("提示：(输入选择0或1并按ENTER建结束输入，输入\'2\'结束程序)\n");
        scanf("%d", &selection);
        while (selection != 0 && selection != 1 && selection != 2)
        {
            while ((getchar()) != '\n')
                continue;
            printf("请输入数字0,1或2，并按ENTER键结束输入!\n");
            printf("你选择的服务是：");
            scanf("%d", &selection);
        }
    }
    printf("谢谢使用！\n");
    system("pause");
    return 0;
}

void collectData(char filename[], struct character *collectTable[], int *size) //一个字符串即压缩文件的名称
{

    int i;
    struct character *tmpCell;
    FILE *pf = fopen(filename, "r");
    char ch;
    if (pf == NULL)
    {
        printf("未找到该文件！\n");
        printf("文件可能不存在或程序与文件不在同一文件目录下\n");
        printf("请重新启动程序重试\n");
        printf("谢谢使用！\n");
        exit(0);
    }
    else
    {
        while ((ch = fgetc(pf)) != EOF) //O(N)
        {
            // putchar(ch);
            if (*size == 0) //为空的时候直接插入
            {
                collectTable[0] = malloc(sizeof(struct character)); //collextTable[0]是个哑结点
                tmpCell = malloc(sizeof(struct character));         //忽略了错误检测
                tmpCell->dumb = 0;
                tmpCell->name = ch;
                tmpCell->frequency = 1;
                tmpCell->left = NULL;
                tmpCell->right = NULL;
                collectTable[1] = tmpCell;
                *size += 1;
            }
            else
            {
                for (i = 1; i <= *size; i++) //O(M)
                {
                    if (collectTable[i]->name == ch)
                    {
                        collectTable[i]->frequency += 1;
                        break;
                    }
                }

                if (i > *size) //说明没有找到
                {
                    tmpCell = malloc(sizeof(struct character)); //忽略了错误检测
                    tmpCell->name = ch;
                    tmpCell->dumb = 0;
                    tmpCell->frequency = 1;
                    tmpCell->left = NULL;
                    tmpCell->right = NULL;
                    collectTable[i] = tmpCell;
                    *size += 1;
                }
            }
        }
    }

    fclose(pf);
}

priorityQueue buildHeap(struct character *collectTable[], int *size)
{

    priorityQueue Q;
    int i;

    for (i = *size / 2; i > 0; i--)
    {
        downFiler(i, collectTable, size);
    }
    collectTable[0]->frequency = *size; //保存堆中元素的数量
    Q = collectTable;
    return Q;
}

void downFiler(int i, priorityQueue collectTable, int *size)
{

    int child;
    queueNode tmpCell;

    tmpCell = collectTable[i];
    // printf("size = %d\n", *size);
    for (child = i * 2; child <= *size; child *= 2)
    {
        if (child != *size &&
            collectTable[child]->frequency > collectTable[child + 1]->frequency)
            child = child + 1;

        if (tmpCell->frequency > collectTable[child]->frequency)
        {
            collectTable[i] = collectTable[child];
            i = child;
        }
        else
            break;
    }
    collectTable[i] = tmpCell;
}

queueMin deleteMin(priorityQueue Q, int *size) //删除一个元素记得把Q[0]->frequency或collectTableSize-1
{
    queueMin Node;
    int lastest = *size; //最末尾元素的索引

    Node = Q[1];
    Q[1] = Q[lastest];
    *size -= 1; //堆内元素减一
    Q[0]->frequency -= 1;
    downFiler(1, Q, size);
    return Node;
}

void insert(queueNode q, priorityQueue Q, int *size) //合并的元素插入堆
{

    int i;
    *size += 1;
    Q[0]->frequency += 1;
    Q[*size] = q;
    for (i = *size / 2; i > 0; i--)
        downFiler(i, Q, size);
}

huffmanTree buildHuffmanTree(priorityQueue Q, huffmanTree T, int *size) //一直保持树的右子树不小于左子树
{

    queueMin first;
    queueMin second; //两次DeleteMin所得的权重最小的元素
    dumb dumbNode;

    while (*size != 1)
    {
        first = deleteMin(Q, size);
        second = deleteMin(Q, size);

        // printf("first (dumb%d) %c : %d \n", first->dumb, first->name, first->frequency);
        // printf("second (dumb%d) %c : %d \n", second->dumb, second->name, second->frequency);
        if (first->dumb != 1 && second->dumb != 1)
        { //这个哑结点会有两个树叶
            dumbNode = malloc(sizeof(struct character));
            if (dumbNode == NULL)
            {
                printf("内存不够，请重新启动程序\n");
                exit(0);
            }
            else
            {
                dumbNode->dumb = 1;
                dumbNode->frequency = first->frequency + second->frequency;
                dumbNode->left = first;
                dumbNode->right = second;
            }
        }
        else
        {
            if (first->dumb == 1)
            {
                dumbNode = malloc(sizeof(struct character));
                if (dumbNode == NULL)
                {
                    printf("内存不够，请重新启动程序\n");
                    exit(0);
                }
                else
                {
                    dumbNode->dumb = 1;
                    dumbNode->frequency = first->frequency + second->frequency;
                    dumbNode->right = first;
                    dumbNode->left = second;
                }
            }
            else if (first->dumb != 1 && second->dumb == 1)
            {
                if (dumbNode == NULL)
                {
                    printf("内存不够，请重新启动程序\n");
                    exit(0);
                }
                else
                {
                    dumbNode = malloc(sizeof(struct character));
                    dumbNode->dumb = 1;
                    dumbNode->frequency = first->frequency + second->frequency;
                    dumbNode->right = second;
                    dumbNode->left = first;
                }
            }
        }

        insert(dumbNode, Q, size);
    }
    T = deleteMin(Q, size);
    return T;
}

void encoding(huffmanTree T, char bitCoding[], char *codingSchedule[], int bits,
              enum direction dire, const int tableSize)
//该例程遍历Huffman树，遍历到非哑结点时将数据记录到散列表
{

    int bts = bits + 1;
    if (T->dumb == 0)
    {
        int index = (int)T->name; //codingSchedule的下标
        // printf("bits = %d\n", bits);
        // printf("%d character %c : frequency %d\n", bits, T->name, T->frequency);
        char *tmp = malloc(sizeof(char) * bts); //tmp是字符串数组，额外的一位是'\0'
        for (int i = 0; i < bits; i++)
            tmp[i] = bitCoding[i]; //将bitCoding的元素全部拷贝到tmp中;
        tmp[bits] = '\0';
        codingSchedule[index] = tmp; //codingSchedule的元素(指向字符数组的指针)指向tmp
    }
    else
    {
        dire = leftTree;
        bitCoding[bits] = '0';
        encoding(T->left, bitCoding, codingSchedule, bts, dire, tableSize);
        dire = rightTree;
        bitCoding[bits] = '1';
        encoding(T->right, bitCoding, codingSchedule, bts, dire, tableSize);
    }
}

void buildCodingSchedule(huffmanTree T, HashTable codingSchedule[],
                         int bits, int *size, const int tableSize)
{

    enum direction dire = leftTree;
    char *bitCoding = malloc(sizeof(char) * (*size * 2)); //防止这个数组可能会越界
    encoding(T, bitCoding, codingSchedule, bits, dire, tableSize);
}

char *compress(char *filename, HashTable codingSchedule[], int *compressNum) //返回压缩了多少的字符
//从原文件得到字符，将对应的Huffman编码每满8位就写入压缩文件中
//如果最后一个字节不满8位，用0填充, 用一个遍历记录原文件有多少字符
//就不用担心最后填充的0解压时会编码成实际字符了
{

    //先打开原文件
    FILE *source = fopen(filename, "r");
    int j = 0;
    char *compressFileName = strcat(filename, "Compressed.txt");
    FILE *target = fopen(compressFileName, "wb");
    char ch;
    int chr;
    int bits;          //二进制位数
    int size;          //每一个字符相对于编码的大小
    int num = 0;       //要转换的十进制数字(每8位就转换成二进制存储到压缩文件)
    char compressChar; //要存入壓縮文件的字符
    int S = 8;
    int i;
    int fNum;
    while ((ch = fgetc(source)) != EOF)
    {
        *compressNum += 1;
        // printf("*compressNum = %d\n", *compressNum);

        chr = (int)ch; //强制类型转换
        i = 0;
        while (codingSchedule[chr][i] != '\0')
        {
            if (codingSchedule[chr][i] == '1')
                num += pow(2, S - 1);
            // printf("S = %d, num = %d\n", S, num);
            S -= 1;
            if (S == 0)
            {
                compressChar = (char)num;
                fNum = fwrite(&compressChar, sizeof(char), 1, target);
                S = 8;
                num = 0;
            }
            i++;
        }
    }
    if (S != 8) //還有尾巴
        fNum = fwrite(&num, sizeof(int), 1, target);
    fclose(source);
    fclose(target);
    return compressFileName;
}

void WriteToTheConfiguration(priorityQueue Q, int collectTableSizeCopy, char *configurationFileName) //写入配置文件的例程
{
    FILE *fp = fopen(configurationFileName, "w");
    for (int i = 1; i <= collectTableSizeCopy; i++)
        fprintf(fp, "%c:%d\n", Q[i]->name, Q[i]->frequency);

    fclose(fp);
}

priorityQueue ReadConfigurationFile(char *ConfigurationFile, int *sum, int *collectTableSize) //返回字符的频率总会，为解码的时候排除附加的0
{
    int i = 0;
    int y = 0;
    priorityQueue Result;
    int fre; //准换成数字的frequency
    char before;
    char ch; //存储冒号之前的字符
    struct character *tmpCell;
    ConfigurationFile = strcat(ConfigurationFile, "ConfigurationFile.txt");
    // printf("0conpressFileName = %s\n", ConfigurationFile);
    FILE *pf = fopen(ConfigurationFile, "r");
    int colon = 0; //0代表冒号之前，1代表冒号之后
    int chrI = 0;  //字符强制类型准换的十进制值
    struct character *result[characterKind + 1];
    char frequency[20];                           //存储冒号之后的字符
    result[0] = malloc(sizeof(struct character)); //result[0]是个哑结点
    if (result[0] == NULL)
    {
        printf("内存不够!!!\n");
        printf("请重启程序重试\n");
        exit(0);
    }
    y++; //数组从result[y(1)]开始存储数据
    //在这里直接建树怎么样
    if (pf == NULL)
    {
        printf("未找到配置文件!\n");
        printf("文件解压失败，请退出程序重试!\n");
        printf("谢谢使用\n");
        exit(0);
    }
    else
    {
        while ((ch = fgetc(pf)) != EOF)
        {
            if (ch == ':')
            {
                colon = 1;
                continue;
            }
            if (colon == 0)
                before = ch;
            if (colon == 1)
            {
                frequency[i] = ch;
                i++;
                while ((ch = fgetc(pf)) != '\n')
                { //可以忽略掉每一行最后的多余的'\n',作用就可以选出我们需要记录的'\n'
                    frequency[i] = ch;
                    i++;
                }
                frequency[i] = '\0'; //转换为字符串
                sscanf(frequency, "%d", &fre);
                tmpCell = malloc(sizeof(struct character));
                tmpCell->dumb = 0;
                tmpCell->name = before;
                tmpCell->frequency = fre;
                tmpCell->left = NULL;
                tmpCell->right = NULL;
                result[y++] = tmpCell;
                (*collectTableSize)++;
                // printf("0collectTableSize = %d\n", *collectTableSize);
                // printf("result[y - 1]->name-> %c:%d\n", result[y - 1]->name, result[y - 1]->frequency);
                //这里就可以放入数组中
                *sum += fre;
                colon = 0;
                i = 0;
            }
        }
        result[0]->frequency = *sum; //记录字符频率的总数
    }
    Result = result;
    return Result;
}

int pop(Stack S)
{
    Postion p;
    int element;
    // printf("H\n");
    // printf("S->%d\n", S->next->element);
    if (S->next == NULL)
    {
        printf("栈为空！！！\n");
        exit(0);
    }
    element = S->next->element;
    // printf("element = %d\n", element);
    p = S->next;
    S->next = S->next->next;
    p->next = NULL;
    S->element -= 1;
    free(p);
    // printf("G\n");
    return element;
}

void push(int element, Stack S)
{
    Postion p;
    SNode tmpCell;

    p = S->next;
    tmpCell = malloc(sizeof(struct stackNode));
    tmpCell->element = element;
    S->next = tmpCell;
    tmpCell->next = p;
    S->element += 1;
}

int isEmpty(Stack S)
{
    return (S->element == 0) ? 1 : 0;
}

char *decompression(char *filename, huffmanTree T, int *sum)
{
    //我需要两个栈
    int i = 0;
    int j = 0;
    int y = 0;
    int enterline = 0;
    int popNum;
    Stack bit = NULL;
    // int bit[8];                                  //八位的bitValue每一位的二进制值
    int Valuedecimal;                           //字符的十进制
    char bitValue;                              //读取的二进制数的值
    char character;                             //要写入解压文件的字符
    FILE *pCompressFile = fopen(filename, "r"); //以二进制模式打开文件
    FILE *resultFP = NULL;                      //指向解压后文件的指针
    huffmanTree t = T;                          //指向树的结点的指针，每次解码完一个字符后重新指向树根
    char *resultFileName = malloc(sizeof(char) * 30);
    if (pCompressFile == NULL)
    {
        printf("Out of space!!!\n");
        exit(0);
    }
    //设计好解压后的文件名
    while (filename[i] != '.')
    {
        resultFileName[i] = filename[i];
        i++;
    }
    j = i;
    resultFileName[j++] = '@';
    while (filename[i] != 'C')
    {
        resultFileName[j] = filename[i];
        j++;
        i++;
    }
    resultFileName[j] = '\0';

    //栈的初始化
    bit = malloc(sizeof(struct stackNode));
    bit->next = NULL;
    bit->element = 0;

    resultFP = fopen(resultFileName, "w");
    if (resultFP == NULL)
    {
        printf("解压失败，请重启程序重试！\n");
        exit(0);
    }
    else
    { //开始解码
        while (*sum >= 0)
        {
            y = 0;
            //读取二进制数
            Valuedecimal = fgetc(pCompressFile);
            if (Valuedecimal == EOF)
                break;

            //读取一字节(8位)的字符
            while (bit->element != 8 && Valuedecimal != 0)
            {
                // printf("1Valuedecimal.txt = %d\n", Valuedecimal);
                push(Valuedecimal % 2, bit);
                Valuedecimal = Valuedecimal / 2;
                y++;
            }
            while (y < 8)
            {
                push(0, bit);
                y++;
            }
            //遍历二进制数组
            while (!isEmpty(bit))
            {
                if (t->dumb == 0)
                {
                    character = t->name;
                    t = T;                              //重新指向树根
                    fprintf(resultFP, "%c", character); //写入文件
                    *sum = (*sum) - 1;
                }
                else
                {
                    popNum = pop(bit);
                    printf("%d", popNum); //打印文件的二进制值
                    enterline++;
                    if (enterline % 4 == 0)
                        printf(" ");
                    if (popNum == 0)
                    {
                        t = t->left;
                    }
                    else if (popNum == 1)
                    {
                        t = t->right;
                    }
                }
            }
        }
    }

    fclose(pCompressFile);
    fclose(resultFP);
    return resultFileName;
}
//压缩
void Compress()
{
    int i = 0;
    int bits = 0; //一个count数记录编码,每一次在Huffman树中找到字符时放进一个encoding
    const int tableSize = characterKind - 1;
     //collectTable[0]不放元素,不过在建堆后可以存储堆中元素的数量
    int collectTableSize = 0; //记录collectTable装了多少种字符了，其实是0
    int collectTableSizeCopy;
    huffmanTree T;
    struct character *collectTable[characterKind + 1]; //记录每个字符出现频率的数组
    HashTable codingSchedule[characterKind - 1];       //ASCII一共有127个字符以0开头，这个就是一个简单的数组，不是散列表，不像该HashTable了
    char *filename = malloc(sizeof(char) * 30);
    char *filename1 = malloc(sizeof(char) * 30);
    char configurationFileName[20] = "";
    int compressNum = 0; //记录压缩了多少的字符
    char *compressFileName;
    priorityQueue Q = NULL;

    printf("请输入要压缩的文件名(带文件后缀以ENTER建结束输入):\n");
    scanf("%s", filename);
    strcpy(filename1, filename);
    collectData(filename, collectTable, &collectTableSize);
    Q = buildHeap(collectTable, &collectTableSize);
    //将Q写入一个配置文件
    collectTableSizeCopy = collectTableSize; //记录collectTable装了多少种字符了，初始是0
    strcpy(configurationFileName, filename);
    strcat(configurationFileName, "Compressed.txtConfigurationFile.txt");
    WriteToTheConfiguration(Q, collectTableSizeCopy, configurationFileName);
    // for (i = 1; i <= collectTableSize; i++)
    // {
    //     printf("Q[%d]'s name is %c and frequency is %d\n", i, Q[i]->name, Q[i]->frequency);
    // } //遍历堆
    // printf("2filename = *%s*\n", filename);
    T = buildHuffmanTree(Q, T, &collectTableSize);
    buildCodingSchedule(T, codingSchedule, bits, &collectTableSizeCopy, tableSize);
    /*开始压缩*/
    // printf("P: %s\n", codingSchedule[80]);
    compressFileName = compress(filename, codingSchedule, &compressNum); //返回压缩的文件
    printf("压缩完成!!!\n");
    printf("总共压缩了%d个字符\n", compressNum);
    printf("压缩后的文件名为%s\n", compressFileName);
    free(T);
}

void Decompress()
{
    int i = 0;
    int sum = 0;              //读取的字符频率值的总和
    int bits = 0;             //一个count数记录编码,每一次在Huffman树中找到字符时放进一个encoding
    int collectTableSize = 0; //记录collectTable装了多少种字符了，其实是0
    huffmanTree T= NULL;
    priorityQueue Q;
    char *resultFileName; //解压后的文件名
    char compressFileName[30];
    char configurationFileName[50];
    HashTable codingSchedule[characterKind - 1]; //ASCII一共有127个字符以0开头，这个就是一个简单的数组，不是散列表，不像该HashTable了

    printf("请输入要解压的文件名(以ENTER建结束输入)\n");
    scanf("%s", compressFileName);
    while (compressFileName[i] != '\0')
    {
        configurationFileName[i] = compressFileName[i];
        i++;
    }
    configurationFileName[i] = '\0';
    printf("开始解压！\n");
    printf("正在读取配置文件...\n");
    Q = ReadConfigurationFile(configurationFileName, &sum, &collectTableSize); //这个数组就是优先队列
    T = buildHuffmanTree(Q, T, &collectTableSize);                             //重建了Huffman树
    //开始以二进制读取压缩文件，并按树解码，sum是字符总数
    // printf("%c\n", T->right->right->right->left->right->right->right->left->right->name);
    // printf("1conpressFileName = %s\n", compressFileName);1
    printf("读取配置文件成功！\n");
    printf("正在解压...\n");
    resultFileName = decompression(compressFileName, T, &sum);
    printf("解压成功!!!\n");
    printf("解压后的文件名为:%s\n", resultFileName);
    free(T);
    
}