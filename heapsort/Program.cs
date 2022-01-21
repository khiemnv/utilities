using System;

namespace heapsort
{
    public class MyHeap<T>
    {
        T[] h;
        Func<T, T, int> comp;
        int len;
        public MyHeap(T[] arr, Func<T, T, int> comp)
        {
            h = arr;
            this.comp = comp;
            len = arr.Length;
            BuildMinHeap();
        }

        void BuildMinHeap()
        {
            for (int i = (len / 2) - 1; i >= 0; i--)
            {
                MinHeap(i);
            }
        }
        void MinHeap(int i)
        {
            int largest = i;
            int l = (i * 2) + 1;
            int r = (i * 2) + 2;
            if (l < len && (comp(h[l], h[largest]) > 0))
            {
                largest = l;
            }

            if (r < len && (comp(h[r], h[largest]) > 0))
            {

                largest = r;
            }

            if (largest != i)
            {
                swap(i,largest);
                MinHeap(largest);
            }
        }
        void swap(int i, int m)
        {
            T tmp = h[m];
            h[m] = h[i];
            h[i] = tmp;
        }
        public T PopMin()
        {
            T tmp = h[0];
            len--;
            swap(0, len);
            MinHeap(len);
            return tmp;
        }
    }
    public class HeapSort<T>
    {
        Func<T, T, int> comp;
        T[] h;
        public HeapSort(Func<T, T, int> comp)
        {

            this.comp = comp;
        }
        public void sort(T[] arr)
        {
            this.h = arr;
            int n = arr.Length;

            // Build heap (rearrange array)
            for (int i = n / 2 - 1; i >= 0; i--)
                heapify(arr, n, i);

            // One by one extract an element from heap
            for (int i = n - 1; i > 0; i--)
            {
                // Move current root to end
                swap(0,i);
                // call max heapify on the reduced heap
                heapify(arr, i, 0);
            }
        }
        void swap(int i, int m)
        {
            T tmp = h[m];
            h[m] = h[i];
            h[i] = tmp;
        }
        // To heapify a subtree rooted with node i which is
        // an index in arr[]. n is size of heap
        void heapify(T[] arr, int n, int i)
        {
            int largest = i; // Initialize largest as root
            int l = 2 * i + 1; // left = 2*i + 1
            int r = 2 * i + 2; // right = 2*i + 2

            // If left child is larger than root
            if (l < n && comp(arr[l], arr[largest]) > 0)
                largest = l;

            // If right child is larger than largest so far
            if (r < n && comp(arr[r], arr[largest]) >0)
                largest = r;

            // If largest is not root
            if (largest != i)
            {
                //int swap = arr[i];
                //arr[i] = arr[largest];
                //arr[largest] = swap;
                swap(i,largest);

                // Recursively heapify the affected sub-tree
                heapify(arr, n, largest);
            }
        }


        // Driver code
    }

    public class program
    {

        /* A utility function to print array of size n */
        static void printArray(int[] arr)
        {
            int n = arr.Length;
            for (int i = 0; i < n; ++i)
                Console.Write(arr[i] + " ");
            Console.Read();
        }
        public static void Main()
        {
            int[] arr = { 12, 11, 13, 5, 6, 7 };
            int n = arr.Length;

            var ob = new HeapSort<int>((x,y) => x-y);
            ob.sort(arr);

            //var h2 = new MyHeap<int>(arr,);
            //for (int i = 0; i < n-1; i++)
            //{
            //    var min = h2.PopMin();
            //}

            Console.WriteLine("Sorted array is");
            printArray(arr);


        }
    }
    // This code is contributed
    // by Akanksha Rai(Abby_akku)
}
