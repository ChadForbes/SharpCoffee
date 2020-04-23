package binarysearch;

public class BinarySearch
{
    public static void main(String[] args)
    {
        int[] arr = { 2, 6, 3, 2, 4, 3, 1, 7, 2, 5 };
        int n = arr.length;
        int x = 4;
        
        int result = Search(arr, 0, n - 1, x);
        
        if (result == -1)
        {
            System.out.println(x + " is not in the aray");
        }
        else
        {
            System.out.println(x + " was found at index " + result + " in the sorted array");
            printArray(arr);
        }
    }
    static int Search(int[] arr, int l, int r, int x)
    {
        BinarySearch bs = new BinarySearch();
        bs.sort(arr);
        if (r >= 1)
        {
            int mid = l + (r - 1) / 2;
            if (arr[mid] == x)
            {
                return mid;
            }
            if (arr[mid] > x)
            {
                return Search(arr, l, mid - 1, x);
            }
            return Search(arr, mid + 1, r, x);
        }
        return -1;
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
