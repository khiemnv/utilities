using System.Collections.Generic;

namespace annotation
{
    public class Labelme
    {
        public class LM_Shape
        {
            public string label { get; set; }
            public List<double[]> points { get; set; }
        }

        public List<LM_Shape> shapes { get; set; }
        public int imageHeight { get; set; }
        public int imageWidth { get; set; }
    }
}
