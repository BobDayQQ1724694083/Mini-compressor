#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/*��Huffman�㷨д��һ������ʵ���ļ���ѹ��(�ͽ�ѹ��)*/
//��ô�������ݣ�����ǣ������ĸתASCII������
//���������

//��������
//����ȡ�����ݷŽ�һ������
//��������Ԫ����ָ�룬ָ��dumb��, name��, frequency�����Ľṹ
//�ڰ���������ɶ����

#define characterKind 128       //7λ���Դ���128���ַ�
typedef struct character *leaf; //�������֮����8/8
struct character
{
    int dumb; //�ж��Ƿ�Ϊ�ƽ��
    char name;
    int frequency;
    leaf left;  //����Ҷ
    leaf right; //����Ҷ
};

//ջ������
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

//�������ռ����������ַ�����ΪM������ΪN������������ʱ��ΪO(N*M)
void collectData(char filename[], struct character *collectTable[], int *size);

//��ʼ����
typedef struct character *queueNode;      //���뵽�ѵ�Ԫ��
typedef struct character **priorityQueue; //�൱��һ��ָ��ָ���ָ�룬��һ������
typedef struct character *queueMin;       //ָ�������С��Ԫ�ص�ָ��
//�������ҪbuildHeap(), deleteMin(),downFilter(), insert()������
priorityQueue buildHeap(struct character *collectTable[], int *size);
queueMin deleteMin(priorityQueue Q, int *size);
void downFiler(int i, struct character *collectTable[], int *size); //��collectTable[i]ָ���Ԫ�����˵���ȷλ��
void insert(queueNode q, priorityQueue Q, int *size);

//��������deleteMin()���ϲ�MIN���ٰѺϲ���Ԫ�طŽ�ȥ,ֱ��deleteMin()Ϊ��
//��ʵ���ִ��N-1�κϲ��ͽ�����,��������collectTableSize��Ϊ����
typedef struct character *dumb; //�ƽ�㣬�����������Ľ��(��������һ������������Ҷ�Ľ��)
//Ҫ����һ�����������¼���ɵ�huffman����

//��ɢ�б�����һ��128��Ԫ�ص�����(����װ��ASCII��������Ԫ��)
//������ÿһ��Ԫ����һ��ָ�룬ָ��һ�����ͱ���encoding
typedef char *HashTable;
//����ɢ�б������

enum direction
{
    leftTree,
    rightTree
}; //�����ж�������Ҷ��������Ҷ,����Ҷ�Ǳ���Ϊ0������Ҷ������1
//1.����huffman��.
//�����ǵĺ���Ϊ����������Ȩֵ�ڵ�,������huffman����;
typedef struct character *huffmanTree;
huffmanTree buildHuffmanTree(priorityQueue Q, huffmanTree T, int *size);
//2.ͨ������������������������;
//�����ǣ��Ӹ��ڵ������������-->0 ��������-->1,
//����Ҷ�ӽڵ��������ͽ�����Ӧ��huffman����д�������С�
void buildCodingSchedule(huffmanTree T, HashTable codingSchedule[], int bits, int *size, const int tableSize);
void encoding(huffmanTree T, char bitCoding[], char *codingSchedule[], int bits, enum direction dire, const int tableSize); //�����̱���Huffman�������������ƽ��ʱ�����ݼ�¼��ɢ�б�

//�����ڽ�ѹ֮ǰҪ��д�����ļ�
//����Ҫ��ѹ�Ļ�����Ҫ֪�����huffman����������ѹ����ʱ����Ҫ��дһ�������ļ���
//�洢huffman������Ϣ�������ַ��Լ��ַ����ֵĴ�������
//�������ļ����潫���ַ�+�ַ����ֵĴ�������һ��
//������Ҫʹ��itoa���������������ת����һ���ַ���(string)���ʹ洢
//ѹ�����ѹ����
char *compress(char *filename, HashTable codingSchedule[], int *compressNum);                         //����ѹ�����ַ�����
void WriteToTheConfiguration(priorityQueue Q, int collectTableSizeCopy, char *configurationFileName); //д�������ļ�������
priorityQueue ReadConfigurationFile(char *ConfigurationFile, int *sum, int *collectTableSize);        //����һ������ѣ�Ϊ�����ʱ���ų����ӵ�08

char *decompression(char *filename, huffmanTree T, int *sum);

void Compress();
void Decompress();

int main()
{
    int selection = 3; //0����ѡ��ѹ����1�����ѹ; ��ʼ��3Ϊ�˽�whileѭ��
    //��������ַ���selection��������Զ�ֹͣ
    while ((selection == 0 || selection == 1) || selection != 2)
    {
        if (selection == 0)
            Compress();
        else if (selection == 1)
            Decompress();

        printf("\n��ѡ�����0.ѹ��    1.��ѹ\n");
        printf("��ʾ��(����ѡ��0��1����ENTER���������룬����\'2\'��������)\n");
        scanf("%d", &selection);
        while (selection != 0 && selection != 1 && selection != 2)
        {
            while ((getchar()) != '\n')
                continue;
            printf("����������0,1��2������ENTER����������!\n");
            printf("��ѡ��ķ����ǣ�");
            scanf("%d", &selection);
        }
    }
    printf("ллʹ�ã�\n");
    system("pause");
    return 0;
}

void collectData(char filename[], struct character *collectTable[], int *size) //һ���ַ�����ѹ���ļ�������
{

    int i;
    struct character *tmpCell;
    FILE *pf = fopen(filename, "r");
    char ch;
    if (pf == NULL)
    {
        printf("δ�ҵ����ļ���\n");
        printf("�ļ����ܲ����ڻ�������ļ�����ͬһ�ļ�Ŀ¼��\n");
        printf("������������������\n");
        printf("ллʹ�ã�\n");
        exit(0);
    }
    else
    {
        while ((ch = fgetc(pf)) != EOF) //O(N)
        {
            // putchar(ch);
            if (*size == 0) //Ϊ�յ�ʱ��ֱ�Ӳ���
            {
                collectTable[0] = malloc(sizeof(struct character)); //collextTable[0]�Ǹ��ƽ��
                tmpCell = malloc(sizeof(struct character));         //�����˴�����
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

                if (i > *size) //˵��û���ҵ�
                {
                    tmpCell = malloc(sizeof(struct character)); //�����˴�����
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
    collectTable[0]->frequency = *size; //�������Ԫ�ص�����
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

queueMin deleteMin(priorityQueue Q, int *size) //ɾ��һ��Ԫ�ؼǵð�Q[0]->frequency��collectTableSize-1
{
    queueMin Node;
    int lastest = *size; //��ĩβԪ�ص�����

    Node = Q[1];
    Q[1] = Q[lastest];
    *size -= 1; //����Ԫ�ؼ�һ
    Q[0]->frequency -= 1;
    downFiler(1, Q, size);
    return Node;
}

void insert(queueNode q, priorityQueue Q, int *size) //�ϲ���Ԫ�ز����
{

    int i;
    *size += 1;
    Q[0]->frequency += 1;
    Q[*size] = q;
    for (i = *size / 2; i > 0; i--)
        downFiler(i, Q, size);
}

huffmanTree buildHuffmanTree(priorityQueue Q, huffmanTree T, int *size) //һֱ����������������С��������
{

    queueMin first;
    queueMin second; //����DeleteMin���õ�Ȩ����С��Ԫ��
    dumb dumbNode;

    while (*size != 1)
    {
        first = deleteMin(Q, size);
        second = deleteMin(Q, size);

        // printf("first (dumb%d) %c : %d \n", first->dumb, first->name, first->frequency);
        // printf("second (dumb%d) %c : %d \n", second->dumb, second->name, second->frequency);
        if (first->dumb != 1 && second->dumb != 1)
        { //����ƽ�����������Ҷ
            dumbNode = malloc(sizeof(struct character));
            if (dumbNode == NULL)
            {
                printf("�ڴ治������������������\n");
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
                    printf("�ڴ治������������������\n");
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
                    printf("�ڴ治������������������\n");
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
//�����̱���Huffman�������������ƽ��ʱ�����ݼ�¼��ɢ�б�
{

    int bts = bits + 1;
    if (T->dumb == 0)
    {
        int index = (int)T->name; //codingSchedule���±�
        // printf("bits = %d\n", bits);
        // printf("%d character %c : frequency %d\n", bits, T->name, T->frequency);
        char *tmp = malloc(sizeof(char) * bts); //tmp���ַ������飬�����һλ��'\0'
        for (int i = 0; i < bits; i++)
            tmp[i] = bitCoding[i]; //��bitCoding��Ԫ��ȫ��������tmp��;
        tmp[bits] = '\0';
        codingSchedule[index] = tmp; //codingSchedule��Ԫ��(ָ���ַ������ָ��)ָ��tmp
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
    char *bitCoding = malloc(sizeof(char) * (*size * 2)); //��ֹ���������ܻ�Խ��
    encoding(T, bitCoding, codingSchedule, bits, dire, tableSize);
}

char *compress(char *filename, HashTable codingSchedule[], int *compressNum) //����ѹ���˶��ٵ��ַ�
//��ԭ�ļ��õ��ַ�������Ӧ��Huffman����ÿ��8λ��д��ѹ���ļ���
//������һ���ֽڲ���8λ����0���, ��һ��������¼ԭ�ļ��ж����ַ�
//�Ͳ��õ����������0��ѹʱ������ʵ���ַ���
{

    //�ȴ�ԭ�ļ�
    FILE *source = fopen(filename, "r");
    int j = 0;
    char *compressFileName = strcat(filename, "Compressed.txt");
    FILE *target = fopen(compressFileName, "wb");
    char ch;
    int chr;
    int bits;          //������λ��
    int size;          //ÿһ���ַ�����ڱ���Ĵ�С
    int num = 0;       //Ҫת����ʮ��������(ÿ8λ��ת���ɶ����ƴ洢��ѹ���ļ�)
    char compressChar; //Ҫ���뉺�s�ļ����ַ�
    int S = 8;
    int i;
    int fNum;
    while ((ch = fgetc(source)) != EOF)
    {
        *compressNum += 1;
        // printf("*compressNum = %d\n", *compressNum);

        chr = (int)ch; //ǿ������ת��
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
    if (S != 8) //߀��β��
        fNum = fwrite(&num, sizeof(int), 1, target);
    fclose(source);
    fclose(target);
    return compressFileName;
}

void WriteToTheConfiguration(priorityQueue Q, int collectTableSizeCopy, char *configurationFileName) //д�������ļ�������
{
    FILE *fp = fopen(configurationFileName, "w");
    for (int i = 1; i <= collectTableSizeCopy; i++)
        fprintf(fp, "%c:%d\n", Q[i]->name, Q[i]->frequency);

    fclose(fp);
}

priorityQueue ReadConfigurationFile(char *ConfigurationFile, int *sum, int *collectTableSize) //�����ַ���Ƶ���ܻᣬΪ�����ʱ���ų����ӵ�0
{
    int i = 0;
    int y = 0;
    priorityQueue Result;
    int fre; //׼�������ֵ�frequency
    char before;
    char ch; //�洢ð��֮ǰ���ַ�
    struct character *tmpCell;
    ConfigurationFile = strcat(ConfigurationFile, "ConfigurationFile.txt");
    // printf("0conpressFileName = %s\n", ConfigurationFile);
    FILE *pf = fopen(ConfigurationFile, "r");
    int colon = 0; //0����ð��֮ǰ��1����ð��֮��
    int chrI = 0;  //�ַ�ǿ������׼����ʮ����ֵ
    struct character *result[characterKind + 1];
    char frequency[20];                           //�洢ð��֮����ַ�
    result[0] = malloc(sizeof(struct character)); //result[0]�Ǹ��ƽ��
    if (result[0] == NULL)
    {
        printf("�ڴ治��!!!\n");
        printf("��������������\n");
        exit(0);
    }
    y++; //�����result[y(1)]��ʼ�洢����
    //������ֱ�ӽ�����ô��
    if (pf == NULL)
    {
        printf("δ�ҵ������ļ�!\n");
        printf("�ļ���ѹʧ�ܣ����˳���������!\n");
        printf("ллʹ��\n");
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
                { //���Ժ��Ե�ÿһ�����Ķ����'\n',���þͿ���ѡ��������Ҫ��¼��'\n'
                    frequency[i] = ch;
                    i++;
                }
                frequency[i] = '\0'; //ת��Ϊ�ַ���
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
                //����Ϳ��Է���������
                *sum += fre;
                colon = 0;
                i = 0;
            }
        }
        result[0]->frequency = *sum; //��¼�ַ�Ƶ�ʵ�����
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
        printf("ջΪ�գ�����\n");
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
    //����Ҫ����ջ
    int i = 0;
    int j = 0;
    int y = 0;
    int enterline = 0;
    int popNum;
    Stack bit = NULL;
    // int bit[8];                                  //��λ��bitValueÿһλ�Ķ�����ֵ
    int Valuedecimal;                           //�ַ���ʮ����
    char bitValue;                              //��ȡ�Ķ���������ֵ
    char character;                             //Ҫд���ѹ�ļ����ַ�
    FILE *pCompressFile = fopen(filename, "r"); //�Զ�����ģʽ���ļ�
    FILE *resultFP = NULL;                      //ָ���ѹ���ļ���ָ��
    huffmanTree t = T;                          //ָ�����Ľ���ָ�룬ÿ�ν�����һ���ַ�������ָ������
    char *resultFileName = malloc(sizeof(char) * 30);
    if (pCompressFile == NULL)
    {
        printf("Out of space!!!\n");
        exit(0);
    }
    //��ƺý�ѹ����ļ���
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

    //ջ�ĳ�ʼ��
    bit = malloc(sizeof(struct stackNode));
    bit->next = NULL;
    bit->element = 0;

    resultFP = fopen(resultFileName, "w");
    if (resultFP == NULL)
    {
        printf("��ѹʧ�ܣ��������������ԣ�\n");
        exit(0);
    }
    else
    { //��ʼ����
        while (*sum >= 0)
        {
            y = 0;
            //��ȡ��������
            Valuedecimal = fgetc(pCompressFile);
            if (Valuedecimal == EOF)
                break;

            //��ȡһ�ֽ�(8λ)���ַ�
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
            //��������������
            while (!isEmpty(bit))
            {
                if (t->dumb == 0)
                {
                    character = t->name;
                    t = T;                              //����ָ������
                    fprintf(resultFP, "%c", character); //д���ļ�
                    *sum = (*sum) - 1;
                }
                else
                {
                    popNum = pop(bit);
                    printf("%d", popNum); //��ӡ�ļ��Ķ�����ֵ
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
//ѹ��
void Compress()
{
    int i = 0;
    int bits = 0; //һ��count����¼����,ÿһ����Huffman�����ҵ��ַ�ʱ�Ž�һ��encoding
    const int tableSize = characterKind - 1;
     //collectTable[0]����Ԫ��,�����ڽ��Ѻ���Դ洢����Ԫ�ص�����
    int collectTableSize = 0; //��¼collectTableװ�˶������ַ��ˣ���ʵ��0
    int collectTableSizeCopy;
    huffmanTree T;
    struct character *collectTable[characterKind + 1]; //��¼ÿ���ַ�����Ƶ�ʵ�����
    HashTable codingSchedule[characterKind - 1];       //ASCIIһ����127���ַ���0��ͷ���������һ���򵥵����飬����ɢ�б������HashTable��
    char *filename = malloc(sizeof(char) * 30);
    char *filename1 = malloc(sizeof(char) * 30);
    char configurationFileName[20] = "";
    int compressNum = 0; //��¼ѹ���˶��ٵ��ַ�
    char *compressFileName;
    priorityQueue Q = NULL;

    printf("������Ҫѹ�����ļ���(���ļ���׺��ENTER����������):\n");
    scanf("%s", filename);
    strcpy(filename1, filename);
    collectData(filename, collectTable, &collectTableSize);
    Q = buildHeap(collectTable, &collectTableSize);
    //��Qд��һ�������ļ�
    collectTableSizeCopy = collectTableSize; //��¼collectTableװ�˶������ַ��ˣ���ʼ��0
    strcpy(configurationFileName, filename);
    strcat(configurationFileName, "Compressed.txtConfigurationFile.txt");
    WriteToTheConfiguration(Q, collectTableSizeCopy, configurationFileName);
    // for (i = 1; i <= collectTableSize; i++)
    // {
    //     printf("Q[%d]'s name is %c and frequency is %d\n", i, Q[i]->name, Q[i]->frequency);
    // } //������
    // printf("2filename = *%s*\n", filename);
    T = buildHuffmanTree(Q, T, &collectTableSize);
    buildCodingSchedule(T, codingSchedule, bits, &collectTableSizeCopy, tableSize);
    /*��ʼѹ��*/
    // printf("P: %s\n", codingSchedule[80]);
    compressFileName = compress(filename, codingSchedule, &compressNum); //����ѹ�����ļ�
    printf("ѹ�����!!!\n");
    printf("�ܹ�ѹ����%d���ַ�\n", compressNum);
    printf("ѹ������ļ���Ϊ%s\n", compressFileName);
    free(T);
}

void Decompress()
{
    int i = 0;
    int sum = 0;              //��ȡ���ַ�Ƶ��ֵ���ܺ�
    int bits = 0;             //һ��count����¼����,ÿһ����Huffman�����ҵ��ַ�ʱ�Ž�һ��encoding
    int collectTableSize = 0; //��¼collectTableװ�˶������ַ��ˣ���ʵ��0
    huffmanTree T= NULL;
    priorityQueue Q;
    char *resultFileName; //��ѹ����ļ���
    char compressFileName[30];
    char configurationFileName[50];
    HashTable codingSchedule[characterKind - 1]; //ASCIIһ����127���ַ���0��ͷ���������һ���򵥵����飬����ɢ�б������HashTable��

    printf("������Ҫ��ѹ���ļ���(��ENTER����������)\n");
    scanf("%s", compressFileName);
    while (compressFileName[i] != '\0')
    {
        configurationFileName[i] = compressFileName[i];
        i++;
    }
    configurationFileName[i] = '\0';
    printf("��ʼ��ѹ��\n");
    printf("���ڶ�ȡ�����ļ�...\n");
    Q = ReadConfigurationFile(configurationFileName, &sum, &collectTableSize); //�������������ȶ���
    T = buildHuffmanTree(Q, T, &collectTableSize);                             //�ؽ���Huffman��
    //��ʼ�Զ����ƶ�ȡѹ���ļ������������룬sum���ַ�����
    // printf("%c\n", T->right->right->right->left->right->right->right->left->right->name);
    // printf("1conpressFileName = %s\n", compressFileName);1
    printf("��ȡ�����ļ��ɹ���\n");
    printf("���ڽ�ѹ...\n");
    resultFileName = decompression(compressFileName, T, &sum);
    printf("��ѹ�ɹ�!!!\n");
    printf("��ѹ����ļ���Ϊ:%s\n", resultFileName);
    free(T);
    
}