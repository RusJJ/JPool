#ifndef _JPOOL_H
#define _JPOOL_H

#include <stddef.h> // declare NULL

// A simple Pool which gives us a simple pointer to a slot item
template <typename A>
class JPoolDefault
{
public:
    JPoolDefault()
    {
        this->data = NULL;
        this->occupied = NULL;
        this->size = 0;
    }
    JPoolDefault(int itemsCount)
    {
        this->data = new A[itemsCount];
        this->occupied = new bool[itemsCount];
        this->size = itemsCount;
        for(int i = 0; i < itemsCount; ++i) this->occupied[i] = false;
    }
    ~JPoolDefault()
    {
        delete this->data;
        delete this->occupied;
    }
    inline int GetIndex(A* obj)
    {
        int index = obj - this->data;
        return (index < 0 || index >= size) ? -1 : index;
    }
    inline A* AllocAt(int slot, bool reoccupy = false)
    {
        if(reoccupy == false && this->occupied[slot] == true) return NULL;
        this->occupied[slot] = true;
        return &this->data[slot];
    }
    inline void Remove(A* obj)
    {
        int index = GetIndex(obj);
        if(index != -1) this->occupied[index] = false;
    }
    inline void RemoveAt(int slot)       { if(slot < size) this->occupied[slot] = false; }
    inline A*   GetAt(int slot)          { return (this->occupied[slot] == true) ? &this->data[slot] : NULL; }
    inline bool IsSlotOccupied(int slot) { return this->occupied[slot]; }
    inline int  GetSize()                { return this->size; }
    inline bool IsValidPtr(A* ptr)       { return GetIndex(ptr) != -1; }
    inline bool IsAvailable()            { return this->firstFree >= 0 && this->firstFree < this->size; }

protected:
    A*    data;
    bool* occupied;
    int   size;
};



// Still a simple Pool gives us a simple pointer to a slot item
// but also calculates first free slot which allows us to use Alloc()
template <typename A>
class JPoolCalcFree : public JPoolDefault<A>
{
public:
    JPoolCalcFree() : JPoolDefault<A>()
    {
        this->firstFree = -1;
    }
    JPoolCalcFree(int itemsCount) : JPoolDefault<A>(itemsCount)
    {
        this->firstFree = 0;
    }
    inline A* Alloc()
    {
        if(this->firstFree >= this->size) return NULL;

        A* ret = &this->data[this->firstFree];
        this->occupied[this->firstFree] = true;
        while(++this->firstFree < this->size)
        {
            if(this->occupied[this->firstFree] == false) break;
        }
        return ret;
    }
    inline A* AllocAt(int slot, bool reoccupy = false)
    {
        if(reoccupy == false && this->occupied[slot] == true) return NULL;
        if(this->firstFree == slot)
        {
            while(++this->firstFree < this->size)
            {
                if(this->occupied[this->firstFree] == false) break;
            }
        }
        this->occupied[slot] = true;
        return &this->data[slot];
    }
    inline void Remove(A* obj)
    {
        int index = GetIndex(obj);
        if(index != -1)
        {
            this->occupied[index] = false;
            if(index < this->firstFree) this->firstFree = index;
        }
    }
    inline void RemoveAt(int slot)
    {
        if(slot < this->size)
        {
            this->occupied[slot] = false;
            if(slot < this->firstFree) this->firstFree = slot;
        }
    }
    inline int GetFirstFreeSlot() { return this->firstFree; }

protected:
    int firstFree;
};



// Still a simple Pool gives us a simple pointer to a slot item
// but also calculates the highest slot ever used (for optimization purposes)
template <typename A>
class JPoolCalcHighest : public JPoolDefault<A>
{
public:
    JPoolCalcHighest() : JPoolDefault<A>()
    {
        this->highestSlotUsedEver = -1;
    }
    JPoolCalcHighest(int itemsCount) : JPoolDefault<A>(itemsCount)
    {
        this->highestSlotUsedEver = -1;
    }
    inline A* AllocAt(int slot, bool reoccupy = false)
    {
        if(reoccupy == false && this->occupied[slot] == true) return NULL;
        if(slot > this->highestSlotUsedEver) this->highestSlotUsedEver = slot;

        this->occupied[slot] = true;
        return &this->data[slot];
    }
    inline int GetHighestSlotUsedEver() { return this->highestSlotUsedEver; }

protected:
    int highestSlotUsedEver;
};



// Still a simple Pool gives us a simple pointer to a slot item
// but also calculates the highest slot ever used and first free slot
template <typename A>
class JPoolCalcFreeHighest : public JPoolCalcFree<A>
{
public:
    JPoolCalcFreeHighest() : JPoolCalcFree<A>()
    {
        this->highestSlotUsedEver = -1;
    }
    JPoolCalcFreeHighest(int itemsCount) : JPoolCalcFree<A>(itemsCount)
    {
        this->highestSlotUsedEver = -1;
    }
    inline A* Alloc()
    {
        if(this->firstFree >= this->size) return NULL;
        if(this->firstFree > this->highestSlotUsedEver) this->highestSlotUsedEver = this->firstFree;

        A* ret = &this->data[this->firstFree];
        this->occupied[this->firstFree] = true;
        while(++this->firstFree < this->size)
        {
            if(this->occupied[this->firstFree] == false) break;
        }
        return ret;
    }
    inline A* AllocAt(int slot, bool reoccupy = false)
    {
        if(reoccupy == false && this->occupied[slot] == true) return NULL;
        if(slot > this->highestSlotUsedEver) this->highestSlotUsedEver = slot;
        if(this->firstFree == slot)
        {
            while(++this->firstFree < this->size)
            {
                if(this->occupied[this->firstFree] == false) break;
            }
        }
        this->occupied[slot] = true;
        return &this->data[slot];
    }
    inline int GetHighestSlotUsedEver() { return this->highestSlotUsedEver; }

protected:
    int highestSlotUsedEver;
};

#endif // _JPOOL_H
