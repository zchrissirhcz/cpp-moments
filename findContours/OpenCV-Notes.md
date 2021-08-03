## refs

- https://blog.csdn.net/tanmx219/article/details/82011196

## CvMemStorage

```c++
typedef struct CvMemBlock
{
    struct CvMemBlock*  prev;
    struct CvMemBlock*  next;
}
CvMemBlock;

typedef struct CvMemStorage
{
    int signature;
    CvMemBlock* bottom;           /**< First allocated block.                   */
    CvMemBlock* top;              /**< Current memory block - top of the stack. */
    struct  CvMemStorage* parent; /**< We get new blocks from parent as needed. */
    int block_size;               /**< Block size.                              */
    int free_space;               /**< Remaining free space in current block.   */
}
CvMemStorage;
```

CvMemBlock 双向链表。

CvMemStorage 类似于栈的内存存储描述结构。

bottom: 最底下一个 CvMemBlock。
top：当前（最顶上）一个 CvMemBlock。
当前 block （也就是 top） 可能仍然有空闲空间，而其他 block 则都处于已经分配状态。

**初始化**
```c++
/* Initialize allocated storage: */
static void
icvInitMemStorage( CvMemStorage* storage, int block_size )
{
    if( !storage )
        CV_Error( CV_StsNullPtr, "" );

    if( block_size <= 0 )
        block_size = CV_STORAGE_BLOCK_SIZE;

    block_size = cvAlign( block_size, CV_STRUCT_ALIGN );
    assert( sizeof(CvMemBlock) % CV_STRUCT_ALIGN == 0 );

    memset( storage, 0, sizeof( *storage ));
    storage->signature = CV_STORAGE_MAGIC_VAL;
    storage->block_size = block_size;
}
```

icvInitMemStorage() 用来给已经分配了空间的 CvMemStorage 对象初始化。

```c++
/* Create root memory storage: */
CV_IMPL CvMemStorage*
cvCreateMemStorage( int block_size )
{
    CvMemStorage* storage = (CvMemStorage *)cvAlloc( sizeof( CvMemStorage ));
    icvInitMemStorage( storage, block_size );
    return storage;
}
```
cvCreateMemStorage() 用来创建 CvMemStorage 对象，包括内存空间分配，以及初始化。全局搜索发现， icvInitMemStorage() 仅在此处被调用，因此可直接放到 cvCreateMemStorage() 中展开。



**释放**

```c++
/* Release memory storage: */
CV_IMPL void
cvReleaseMemStorage( CvMemStorage** storage )
{
    if( !storage )
        CV_Error( CV_StsNullPtr, "" );

    CvMemStorage* st = *storage;
    *storage = 0;
    if( st )
    {
        icvDestroyMemStorage( st );
        cvFree( &st );
    }
}
```
cvReleaseMemStorage() 释放 `CvMemStorage**` 指向的内存空间(`CvMemStorage*`)、以及结构体本身(`CvMemStorage**`) 的内存空间。


```c++
/* Release all blocks of the storage (or return them to parent, if any): */
static void
icvDestroyMemStorage( CvMemStorage* storage )
{
    int k = 0;

    CvMemBlock *block;
    CvMemBlock *dst_top = 0;

    if( !storage )
        CV_Error( CV_StsNullPtr, "" );

    // 如果当前 storage 有父节点，那么 dst_top 指向 父storage的top block
    if( storage->parent )
        dst_top = storage->parent->top;

    // 从 bottom block 开始，依次处理每个 block
    for( block = storage->bottom; block != 0; k++ )
    {
        // temp 表示当前处理的 block
        CvMemBlock *temp = block;

        // block 则更新为下一个 block
        block = block->next;
        // 如果当前 storage 有父storage，则把当前 block 挂载到 父storage 上
        if( storage->parent )
        {
            // 具体说来，如果父storage的top不为空，说明父storage至少有一个block，则把当前block挂到父storage的最上面，父storage的top得到更新
            if( dst_top )
            {
                temp->prev = dst_top;
                temp->next = dst_top->next;
                if( temp->next )
                    temp->next->prev = temp;
                dst_top = dst_top->next = temp;
            }
            // 而如果父 storage 的top为空，说明父storage为空，则让父storage的top和bottom都等于当前block
            else
            {
                dst_top = storage->parent->bottom = storage->parent->top = temp;
                temp->prev = temp->next = 0;
                storage->free_space = storage->block_size - sizeof( *temp ); // 这里更新当前storage的空闲可用空间，感觉没啥用。要更新，应该在前一个 if 分支也更新；而由于函数最后设定 free_space 为0,这里显得多此一举。
            }
        }
        // 当前 storage 没有 父storage，则释放当前 block
        else
        {
            cvFree( &temp );
        }
    }

    // 当前storage的top和bottom，都指向NULL；可用空间，清零。
    storage->top = storage->bottom = 0;
    storage->free_space = 0;
}
```


```c++
/* Clears memory storage (return blocks to the parent, if any): */
CV_IMPL void
cvClearMemStorage( TvMemStorage * storage )
{
    if( !storage )
        CV_Error( tv::StsNullPtr, "" );

    // 如果当前 storage 有父storage，则调用 icvDestroyMemStorage （如果有祖父storage，则会挂靠到祖父storage上；没有祖父storage，则父storage会释放）
    if( storage->parent )
        icvDestroyMemStorage( storage );
    else // 当前 storage 没有父 storage，则修改当前 storage 的top block，指向 bottom block；注意，这并没有真的释放内存
    {
        storage->top = storage->bottom;
        storage->free_space = storage->bottom ? storage->block_size - sizeof(TvMemBlock) : 0;
    }
}
```

`cvClearMemStorage()` 的作用很迷惑，到底是要释放，还是只是修改标记、不是真的释放？




## CvMemStoragePos

```c++
typedef struct CvMemStoragePos
{
    CvMemBlock* top;
    int free_space;
}
CvMemStoragePos;
```
CvMemStoragePos 是 CvMemStorage 的一部分，仅仅记录 top CvMemBlock 也就是当前的内存块的地址、当前内存块剩余的空间。

```c++
/* Remember memory storage position: */
CV_IMPL void
cvSaveMemStoragePos( const CvMemStorage * storage, CvMemStoragePos * pos )
{
    if( !storage || !pos )
        CV_Error( CV_StsNullPtr, "" );

    pos->top = storage->top;
    pos->free_space = storage->free_space;
}
```

cvSaveMemStoragePos() 函数，是从 CvMemStorage 结构中，给 CvMemStoragePos 赋值。


```c++
/* Restore memory storage position: */
CV_IMPL void
cvRestoreMemStoragePos( CvMemStorage * storage, CvMemStoragePos * pos )
{
    if( !storage || !pos )
        CV_Error( CV_StsNullPtr, "" );
    if( pos->free_space > storage->block_size )
        CV_Error( CV_StsBadSize, "" );

    /*
    // this breaks icvGoNextMemBlock, so comment it off for now
    if( storage->parent && (!pos->top || pos->top->next) )
    {
        CvMemBlock* save_bottom;
        if( !pos->top )
            save_bottom = 0;
        else
        {
            save_bottom = storage->bottom;
            storage->bottom = pos->top->next;
            pos->top->next = 0;
            storage->bottom->prev = 0;
        }
        icvDestroyMemStorage( storage );
        storage->bottom = save_bottom;
    }*/

    storage->top = pos->top;
    storage->free_space = pos->free_space;

    if( !storage->top )
    {
        storage->top = storage->bottom;
        storage->free_space = storage->top ? storage->block_size - sizeof(CvMemBlock) : 0;
    }
}
```

cvRestoreMemStoragePos() 函数，是从 CvMemStoragePos 结构中恢复 CvMemStorage 的值。然而目前的实现有点 bug，只要 top 为 NULL ，就从 bottom 赋值。那中间那些 block 呢？被忽略了，感觉不妥。


---

20210628  16:43

现在理解了， TvSeq 是基类， TvChain, TvContours,  TvSet 等都是它的子类。 使用 C 的方式实现了继承。没见过的话确实不理解。




`tvFindContours_Impl()` 的关键三个函数：
- tvStartFindContours()
- tvFindNextContour()
- tvEndFindContours()