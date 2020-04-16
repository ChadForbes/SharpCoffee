package insertionsort;
public class InsertionSort
{
    public static void main(String[] args)
    {
        int[] arr = { 2, 6, 3, 2, 4, 3, 1, 7, 2, 5 };
        printArray(arr);
        InsertionSort inSort = new InsertionSort();
        inSort.sort(arr);
        printArray(arr);
    }
    void sort(int[] arr)
    {
        int n = arr.length;
        for (int i = 1; i < n; i++)
        {
            int key = arr[i];
            int j = i - 1;
            while (j >= 0 && arr[j] > key)
            {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }
    }
    static void printArray(int[] arr)
    {
        int n = arr.length;
        for (int i = 0; i < n; i++)
        {
            System.out.print(arr[i] + " ");
        }
        System.out.print("\n");
    }
}
