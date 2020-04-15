![](OpenCL.png)

Pentru a implementa un algoritm cu ajutorul OpenCL, avem nevoie de două programe: un program care rulează pe
procesorul gazdă ( de obicei scris in C/C++ dar exista adaptari si in python ), şi un program care va rula pe GPU ( scris in 
limbajul CL, care e un subset de C99 cu unele modificari )

În afară de tipurile de memorie prezentate anterior, limbajul CL nu suportă funcţii recursive şi pointeri către funcţie.
În cadrul unui program CL, funcţiile care pot fi invocate poartă identificatorul kernel şi au tipul void.
```void kernel add(global int* a,global* int b) {....}```  -> Un program CL poate avea mai multe funcţii de tip kernel.

În limbajul CL sunt definite nişte tipuri de date noi numite vectori, şi anume:
charn, ucharn, shortn, ushortn, intn, uintn, longn, ulongn, floatn; unde n poate fi 2,4,8 şi 16.
```float4 pos = (float4)(1.0 , 2.0 , 3.0 , 4.0);```
Pentru a accesa un element dintr-un astfel de vector putem folosi coordonatele xywz  sau indexul numeric.
``` float x_coord = pos.x; sau float x_coord = pos.s0;```

Pentru vectori mai mari de 4 elemente nu se mai pot folosi coordonatele, iar indexul se scrie în hexazecimal.
```float16 some_vector = (float16)(1.0); float a = some_vector.sA;```
Vectorul some_vector are toate elemententele cu valoarea 1, iar a va fi egal cu al 11-lea element din some_vector.



Limbajul CL suportă extensii.  De exemplu ```“#pragma OPENCL EXTENSION cl_extension_name : enable”``` va activa extensia 
cu numele cl_extension_name. 
Prin activarea acestei extensii se defineşte tipul double şi tipul de vector doublen.

Limbajul CL suportă un tip de numere cu virgulă mobilă cu precizie redusă, care se numeşte half. Pentru a stoca un 
număr half este nevoie de 16 biţi. Pentru a putea folosi numere de tip half trebuie activată extensia cl_khr_fp16. După 
activarea acestei extensii se va defini şi vectorul halfn , unde n poate avea valorile 2,4,8 şi 16.


Un program CL poate rula atât pe un dispozitiv cu ordine little endian, cât şi big endian. Pentru a determina ordinea de memorie a 
dispozitivului se poate testa prezenţa ```macro-ului __ENDIAN_LITTLE__. ``` De exemplu:
```
#ifdef __ENDIAN_LITTLE__
ushort htons(ushort s){return ((s & 0xff) << 8) | (s >> 8);} 
#else
ushort htons(ushort s){return s;} 
#endif
```
În codul de mai sus se defineşte funcţia htons care traduce un număr din ordinea dispozitivului în ordinea de reţea (big endian).





În OpenCL avem noţiunile de platformă (cl_platform_id) şi dispozitiv (cl_device_id).
O platformă poate avea mai multe dispozitive. De exemplu dacă într-un computer avem două sau mai multe GPU-uri de la acelaşi 
producător. Pentru a afla numărul de platforme disponibile în sistem folosim următorul cod:
```
cl_uint platformCount;
cl_int result = clGetPlatformIDs(0, NULL, &platformCount);
```

În urma acestul apel, platformCount va avea conţine numărul de platforme disponibile, iar result va fi egal cu CL_SUCCESS.
Dacă computerul nu are plaforme disponibile, result va fi egal cu CL_INVALID_VALUE.

Pentru a obţine numărul de dispozitive disponibile dintr-o anumită platformă, folosim codul:
```
cl_uint deviceCount;
result = clGetDeviceIDs(platforms[i],CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);
```
platforms este vectorul de platforme construit anterior, iar i este un index oarecare din acest vector.
În urma acestul apel, deviceCount va avea conţine numărul de dispozitive disponibile, iar resut va fi egal cu 
CL_SUCCESS


Programul multithreaded a fost implementat cu biblioteca pthread.
Pentru a determina numărul de threaduri ale procesorului s-a folosit umătorul cod: int thread_num = std::thread::hardware_concurrency();
thread_num va fi numărul de threaduri disponibile.

Alocarea şi iniţializarea rămân aceleaşi ca în programul single-threaded.
Se defineşte o structură struct thread_argv, care conţine două variabile de tip întreg: start si stop.

Fiecare thread primeşte o parte din vector de calculat (thread_space). Este foarte probabil ca N să nu se împartă perfect la 
numărul de threaduri, aşa că partea cea mai mare o va primi ultimul thread.
```
unsigned int thread_space = N / thread_num; 
for(unsigned int i=0; i<thread_num; i++)
{
t_info[i].start = thread_space * i;
t_info[i].stop = thread_space * (i+1); 
if(i == thread_num - 1) {t_info[i].stop = N;
}
```


Scopul acestui test este să se ia o imagine color şi să se salveze în alb-nergru.
Imaginea de intrare este în format RGB24 bmp. Imaginea de ieşire este tot în acelaşi format.
Pentru a stoca o imagine alb-negru în format RGB24 va trebui să avem valori egale pentru R,G şi B.

R = red; G = green; B = blue; 
Practic un pixel este stocat folosind o pereche de 3 octeţi (un octet pentru R, unul pentru G şi unul pentru B).

Notăm cu Y(gamma) valoarea unică a pixelului alb-negru din imaginea de ieşire.
Adică R_ieşire = Y, G_ieşire = Y şi B_ieşire = Y.

Pentru a calcula Y, am putea face media aritmetică a lui R,G,B;
Acest lucru va produce o imagine de ieşie destul de bună, dar se poate şi mai bine.


Pentru programul multithreaded se citeşte fişierul în memorie, se alocă memorie pentru imaginea alb-negru şi se fac 
verficările la fel ca la programul single threaded.
Pentru a lucra cu threaduri  s-a folosit bilbioteca pthread.
Algoritmul este asemănător cu cel de la programul vector multithreaded.

Se calculează o bucată de lucru pentru ficare thread, iar ultimul thread primeşte partea cea mai mare:
```
int N = image_w * image_h;
unsigned int thread_space = N / thread_num;
for(unsigned int i=0; i<thread_num; i++) {
t_info[i].start = thread_space * i; t_info[i].stop = thread_space * (i+1); 
if(i == thread_num - 1) {t_info[i].stop = N;}
pthread_create(&threads[i],NULL,thread_proc,(void*)&t_info[i]);}
```

# Imagini
Rez  | Single     | Multi         | OpenCl
-----| -----------| --------------| ------------
VGA  | 4.19(238)  |  0.92(1084)   | 0.093 (10730)
HD   | 9.26(107)  |  2.13(467)    | 0.29  (3350)
FHD  | 18.38(54)  |  4.88(204)    | 0.58  (1720)
QHD  | 32.08(31)  |  10.84(92)    | 1.04  (960)
4K   | 73.21(13)  |  21.00(47)    | 2.27  (440)
16K  | 1414(0.70) |  245(4)       | 26.31 (38)



# Vectori
N       | Single | Multi | OpenCl
--------|--------|-------| --------
10000   | 0.20   | 0.05  | 0.04
100000  | 1.92   | 0.51  | 0.73
1000000 | 26.43  | 16.33 | 9.71
