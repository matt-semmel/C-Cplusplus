public interface MemSim
{
    public String toString();

    public void simulate(char accessType, long address, int process, int line);
}
