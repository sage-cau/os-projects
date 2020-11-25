#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <libgen.h> // basename() 함수의 라이브러리

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
        //open을 실패한 경우 혹시 directory는 아닌지 한번 더 확인
        if((pdir = opendir(argv[2])) <= 0) {
            perror(argv[2]);
            return -1;
        }
        closedir(pdir);
    }

    fstat(in, &statBuf);
    //source가 directory 일 때
    if(S_ISDIR(statBuf.st_mode)) {  
        fstat(out, &statBuf);
        //destination이 directory 일 때
        if(S_ISDIR(statBuf.st_mode)) {  
            copyFromDirectory(argv[1], argv[2]); // 재귀호출을 사용하기 위해 따로 함수를 생성함
        }
        //destination이 regular file 일 때
        else if(S_ISREG(statBuf.st_mode)) {
            write(2, "Usage: copy file1 file2 | copy dir1 dir2 | copy file dir \n", 58);
            return -1;
        }
    }
    //source가 regular file 일 때
    else{       
        stat(argv[2], &statBuf);
        //destination이 directory 일 때
        if(S_ISDIR(statBuf.st_mode)) { 
            // regular file로 끝나는 형식의 out 경로를 생성
            strcpy(outPath, argv[2]);
            strcat(outPath, "/");
            strcat(outPath, basename(argv[1])); // basename: path를 파일이름으로 변환해주는 함수
            
            if ((outFile = open(outPath, O_WRONLY|O_CREAT|O_TRUNC , S_IRUSR|S_IWUSR)) < 0) {
                perror(outPath);
                return -1;
            }
            while((n = read(in, buf, sizeof(buf))) > 0)
                write(outFile, buf, n); // 파일 복사

            close(outFile);
        }
        //destination이 regular file 일 때
        else if(S_ISREG(statBuf.st_mode)) {
            while((n = read(in, buf, sizeof(buf))) > 0)
                write(out, buf, n);     // 파일 복사
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

    //(반복문) 현재 디렉토리의 내용을 하나씩 순서대로 newOutPath로 복사
    while((dirt = readdir(pdir)) != NULL) {
        // newInPath: "inPath경로/현재디렉토리이름"
        strcpy(newInPath, inPath);
        strcat(newInPath, "/");
        strcat(newInPath, dirt->d_name);
        lstat(newInPath, &statBuf); 
        // 파일이 디렉토리인 경우
        if(S_ISDIR(statBuf.st_mode)) {
            if(strcmp(dirt->d_name, ".") && strcmp(dirt->d_name, "..")) {
                // newOutPath: "outPath경로/현재디렉토리이름"
                strcpy(newOutPath, outPath);
                strcat(newOutPath, "/");
                strcat(newOutPath, dirt->d_name);

                mkdir(newOutPath, S_IRUSR|S_IWUSR|S_IXUSR); //mkdir: 디렉토리 생성
                copyFromDirectory(newInPath, newOutPath);   // 재귀 호출로 sub directory도 복사한다.
            }
        }
        // 파일이 일반 파일인 경우
        else {
        //현재 파일을 destination 으로 복사
            if ((in = open(inPath, O_RDONLY )) < 0) {
                perror(inPath);
                return -1;
            }
            // newOutPath: "outPath경로/현재파일이름"
            strcpy(newOutPath, outPath);
            strcat(newOutPath, "/");
            strcat(newOutPath, dirt->d_name);
            if ((outFile = open(newOutPath, O_WRONLY|O_CREAT|O_TRUNC , S_IRUSR|S_IWUSR)) < 0) {
                perror(outPath);
                return -1;
            }
            while((n = read(in, buf, sizeof(buf))) > 0)
                write(outFile, buf, n); // 복사
            close(outFile);
            close(in);
        }
    }
    closedir(pdir);
}