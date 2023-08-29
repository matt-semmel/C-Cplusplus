import java.util.LinkedList;

public class PageTableEntry
{
    // Last line that accessed the page
    int lastAccess;
    // Page number
    long pageNum;
    // Flag to note if the page has been written to
    boolean written;
    // List of memory accesses to the page for OPT
    LinkedList<Integer> accesses;

    // Constructor for OPT
    public PageTableEntry(int l, long pn)
    {
        lastAccess = l;
        pageNum = pn;
        written = false;
        accesses = new LinkedList<Integer>();
    }

    // Constructor for LRU
    public PageTableEntry(long pn)
    {
        pageNum = pn;
        written = false;
    }


    public String toString()
    {
        return String.valueOf(pageNum);
    }

}