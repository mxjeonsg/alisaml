
>[!WANING]
> This is work in progress
> The API is higly new and unstable
> There may be some memory leaks

# Alisami

**A xml based app builder**

# Building And running

clone this repo 

``` bash
git clone https://github.com/mxjeonsg/alisaml
```
open de folder and set everythin up
```bash
cd alisaml && mkdir build && cd build
```

compile the project
```bash
cmake .. && cmake --build .
```

run it
```bash
./Alisama
```

>[!NOTE]
>
> If when building you get and error saying that 'allocal.h' could be found
> just go into ```build/_deps/xml-src/test/test-xml-c.c```
> and comment the line with the include ```#include <alloca.h>```


# Help?
If you found a bug or want to contrubuite to this project
see [DOCS](DOCS/doc.md)


# Credits
Idea & Creator [Jeon](https://github.com/mxjeonsg)
Port to Cmake by [Destro](https://github.com/destroK503)