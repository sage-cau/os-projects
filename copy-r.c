#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h> // basename() �Լ��� ���̺귯��

char buf[1024];

int copyFromDirectory(const char *inPath, const char *outPath);

int main (int argc, char **argv)
{
    DIR *pdir;
    int n, in, out, outFile;
    struct stat statBuf;
    char outPath[255];
    
    memset(&statBuf, 0, sizeof(statBuf));
    
    if (argc < 3) {
        write(2, "Usage: copy file1 file2 | copy dir1 dir2 | copy file dir \n", 58);
        return -1;
    }
    //source
    if ((in = open(argv[1], O_RDONLY )) < 0) {
        perror(argv[1]);
        return -1;
    }
    //destination
    if ((out = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) < 0) {
        //open�� ������ ��� Ȥ�� directory�� �ƴ��� �ѹ� �� Ȯ��
        if((pdir = opendir(argv[2])) <= 0) {
            perror(argv[2]);
            return -1;
        }
        closedir(pdir);
    }

    fstat(in, &statBuf);
    //source�� directory �� ��
    if(S_ISDIR(statBuf.st_mode)) {  
        fstat(out, &statBuf);
        //destination�� directory �� ��
        if(S_ISDIR(statBuf.st_mode)) {  
            copyFromDirectory(argv[1], argv[2]); // ���ȣ���� ����ϱ� ���� ���� �Լ��� ������
        }
        //destination�� regular file �� ��
        else if(S_ISREG(statBuf.st_mode)) {
            write(2, "Usage: copy file1 file2 | copy dir1 dir2 | copy file dir \n", 58);
            return -1;
        }
    }
    //source�� regular file �� ��
    else{       
        stat(argv[2], &statBuf);
        //destination�� directory �� ��
        if(S_ISDIR(statBuf.st_mode)) { 
            // regular file�� ������ ������ out ��θ� ����
            strcpy(outPath, argv[2]);
            strcat(outPath, "/");
            strcat(outPath, basename(argv[1])); // basename: path�� �����̸����� ��ȯ���ִ� �Լ�
            
            if ((outFile = open(outPath, O_WRONLY|O_CREAT|O_TRUNC , S_IRUSR|S_IWUSR)) < 0) {
                perror(outPath);
                return -1;
            }
            while((n = read(in, buf, sizeof(buf))) > 0)
                write(outFile, buf, n); // ���� ����

            close(outFile);
        }
        //destination�� regular file �� ��
        else if(S_ISREG(statBuf.st_mode)) {
            while((n = read(in, buf, sizeof(buf))) > 0)
                write(out, buf, n);     // ���� ����
        }
    } 
    close(out);
    close(in);
    return 0;
}



int copyFromDirectory(const char *inPath, const char *outPath) {
    DIR *pdir;
    struct dirent *dirt;
    struct stat statBuf;
    char newInPath[255], newOutPath[255];
    int n, in, outFile;

    memset(&dirt, 0, sizeof(dirt));
    memset(&statBuf, 0, sizeof(statBuf));

    if((pdir = opendir(inPath)) <= 0) {
        perror("opendir");
        return -1;
    }

    //(�ݺ���) ���� ���丮�� ������ �ϳ��� ������� newOutPath�� ����
    while((dirt = readdir(pdir)) != NULL) {
        // newInPath: "inPath���/������丮�̸�"
        strcpy(newInPath, inPath);
        strcat(newInPath, "/");
        strcat(newInPath, dirt->d_name);
        lstat(newInPath, &statBuf); 
        // ������ ���丮�� ���
        if(S_ISDIR(statBuf.st_mode)) {
            if(strcmp(dirt->d_name, ".") && strcmp(dirt->d_name, "..")) {
                // newOutPath: "outPath���/������丮�̸�"
                strcpy(newOutPath, outPath);
                strcat(newOutPath, "/");
                strcat(newOutPath, dirt->d_name);

                mkdir(newOutPath, S_IRUSR|S_IWUSR|S_IXUSR); //mkdir: ���丮 ����
                copyFromDirectory(newInPath, newOutPath);   // ��� ȣ��� sub directory�� �����Ѵ�.
            }
        }
        // ������ �Ϲ� ������ ���
        else {
        //���� ������ destination ���� ����
            if ((in = open(inPath, O_RDONLY )) < 0) {
                perror(inPath);
                return -1;
            }
            // newOutPath: "outPath���/���������̸�"
            strcpy(newOutPath, outPath);
            strcat(newOutPath, "/");
            strcat(newOutPath, dirt->d_name);
            if ((outFile = open(newOutPath, O_WRONLY|O_CREAT|O_TRUNC , S_IRUSR|S_IWUSR)) < 0) {
                perror(outPath);
                return -1;
            }
            while((n = read(in, buf, sizeof(buf))) > 0)
                write(outFile, buf, n); // ����
            close(outFile);
            close(in);
        }
    }
    closedir(pdir);
}