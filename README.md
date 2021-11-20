# C-Study

C언어를 이용해 진행한 계산기, DBMS, 다중 채팅 개인 프로젝트입니다.

#### 계산기 개발 프로젝트
계산기는 () 연산과 -연산 등 기본적인 사칙 연산이 가능합니다. 
데이터 파싱에 대한 기본적인 개념을 숙지하기 위하여 진행한 프로젝트입니다.

##### test case
-------------------------------------------------------------------------------------
-(20-30)-(50-(-40*50)/5+10)*4/100-2
-(10*(3/2)+(1+100/7)/(10+100+(1000+(10/3)-2)))+4
-(5+6)-(4-(9+9*(4-6)))+2

![cal](https://user-images.githubusercontent.com/54059795/142727989-53b35913-d5c9-42a6-9779-ed15b1c37605.PNG)
-----------------------------------------------------------------------------------------

#### 다중 채팅 프로그램
다중 채팅의 경우 리눅스를 기반으로 개발하였습니다.
멀티 스레드 기반의 다중 채팅을 구현하였으며, MonDB를 이용해 채팅 기록 역시 log를 남겼습니다.

##### 다중 채팅
-------------------------------------------------------
![chat](https://user-images.githubusercontent.com/54059795/142727979-c96d3279-959c-4a17-8aa0-d223b1b6f0ac.png)
--------------------------------------------------------

#### DBMS 
MYSQL과 같은 DATABASE 관리 프로그램을 목표로 잡고 개발을 진행하였습니다.
파일 입출력을 통해 실제 DB처럼 동작할 수 있게끔 데이터 log를 저장하고 수정, 삭제, 변경과 같은 기능을 구현하여 보았습니다.
DB에 대한 기본 개념을 공부할 수 있었으며, 구조체의 사용 및 데이터 파싱 능력을 많이 향상시킬 수 있었습니다.

##### DBMS test case
-----------------------------------------------------------------------
CREATE DATABASE testDB1;     
CREATE DATABASE testDB2;       
USE testDB1;      
CREATE TABLE testTB1(id INT(11),title VARCHAR(100),text VARCHAR(50),dec VARCHAR(50));      
CREATE TABLE testTB2(id1 INT(11),title1 VARCHAR(100),text1 VARCHAR(50));      
CREATE TABLE testTB3(id3 INT(11),title3 VARCHAR(100));      
SHOW TABLES;      
SHOW DATABASES;      
INSERT INTO testTB1 (id, title, text, dec) VALUES (552, 'acc', '12', 'hdggggg');      
INSERT INTO testTB1 (id, title, text, dec) VALUES (44, 'ab', '12', 'ddddgg');      
INSERT INTO testTB1 (id, title, text, dec) VALUES (66, 'abc', '123', 'yyyyff');      
INSERT INTO testTB1 VALUES (77, 'abcd', '1234', 'bbbb');      
INSERT INTO testTB1 (id, title, text, dec) VALUES (34,'ss','12','dasfdgg');      
SELECT * FROM testTB1;      
      
SELECT * FROM testTB1 WHERE id = 66 OR text = '12';      
SELECT title, text FROM testTB1;      
SELECT id, dec FROM testTB1;      
SELECT title, text FROM testTB1 WHERE id = 44;      
SELECT * FROM testTB1 WHERE id = 44;      
SELECT id FROM testTB1 WHERE id = 44 OR id != 66;      
SELECT title, text FROM testTB1 WHERE id = 44 OR id != 66;      
SELECT * FROM testTB1 WHERE id != 44 OR id = 66;      
SELECT title, text FROM testTB1 WHERE text = '12' OR id != 66;      
SELECT title, text FROM testTB1 WHERE id = 66 AND id != 44;      
SELECT * FROM testTB1 WHERE id != 66 AND id != 44;      
SELECT * FROM testTB1 WHERE id = 66 AND id != 44;      
      
UPDATE testTB1 SET id = 55 WHERE id = 552;      
UPDATE testTB1 SET text = 'hey' WHERE id = 44;      
UPDATE testTB1 SET text = 'hi' WHERE text = '12';      
UPDATE testTB1 SET id = 11 WHERE id = 44 OR id != 66;      
UPDATE testTB1 SET id = 11 WHERE id = 44 AND id != 66;      
      
DELETE FROM testTB1;      
DELETE FROM testTB1 WHERE id = 66;      
DELETE FROM testTB1 WHERE id = 552;      
DELETE FROM testTB1 WHERE id = 34;      
DELETE FROM testTB1 WHERE id = 66 OR id != 44;      
DELETE FROM testTB1 WHERE id = 66 AND id != 44;      
      
-----------------------------------------------------------------------

##### DBMS
---------------------------------------------------------------------
![DB](https://user-images.githubusercontent.com/54059795/142727965-5d7c0ad3-b2c0-4540-9c06-72867033a4ac.png)
---------------------------------------------------------------------
