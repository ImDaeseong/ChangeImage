using System;
using System.Drawing;
using System.Windows.Forms;

/*
    참조
    SavePng
    https://github.com/techmatt/puppy-planners/blob/c1e45d009811b73117ea2a50fb7b83e5889ffe73/puppy-common/UI/AssetManager.cs
*/

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        private string strPngName;
        private Color backgroundColor;

        public Form1()
        {
            InitializeComponent();
        }
                
        string GetFileExtName(string strFilename)
        {
            int nPos = strFilename.LastIndexOf('.');
            int nLength = strFilename.Length;
            if (nPos < nLength)
                return strFilename.Substring(nPos + 1, (nLength - nPos) - 1);
            return string.Empty;
        }

        string GetFileName(string strFilename)
        {
            int nPos = strFilename.LastIndexOf('\\');
            int nLength = strFilename.Length;
            if (nPos < nLength)
                return strFilename.Substring(nPos + 1, (nLength - nPos) - 1);
            return strFilename;
        }

        string GetFileOnlyName(string strFilename)
        {
            return strFilename.Substring(0, strFilename.LastIndexOf("."));
        }
        
        private void SavePng(string strFileName)
        {                          
            Bitmap OriginBmp = new Bitmap(Image.FromFile(strFileName));
            Bitmap NewBmp = new Bitmap(OriginBmp.Width, OriginBmp.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            backgroundColor = OriginBmp.GetPixel(0, 0);
            const double whitenessThreshold = 0.3;

            Func<Color, Color, double> colorDist = (x, y) =>
            {
                int diffR = x.R - y.R;
                int diffG = x.G - y.G;
                int diffB = x.B - y.B;
                return (Math.Abs(diffR) + Math.Abs(diffG) + Math.Abs(diffB)) / 255.0;
            };

            for (int y = 0; y < OriginBmp.Height; y++)
            {
                for (int x = 0; x < OriginBmp.Width; x++)
                {                    
                    Color color = OriginBmp.GetPixel(x, y);
                    double whiteness = colorDist(color, backgroundColor);

                    NewBmp.SetPixel(x, y, Color.FromArgb(255, color.R, color.G, color.B));

                    if (whiteness < whitenessThreshold)
                    {
                        double alpha = (int)Math.Round(whiteness / whitenessThreshold);
                        if (alpha > 0.0)
                        {
                            int fr = (int)Math.Round((color.R - (1.0 - alpha) * backgroundColor.R) / alpha);
                            int fg = (int)Math.Round((color.R - (1.0 - alpha) * backgroundColor.R) / alpha);
                            int fb = (int)Math.Round((color.R - (1.0 - alpha) * backgroundColor.R) / alpha);
                            NewBmp.SetPixel(x, y, Color.FromArgb(0, fr, fg, fb));
                        }
                        else
                        {
                            NewBmp.SetPixel(x, y, Color.FromArgb(0, backgroundColor.R, backgroundColor.G, backgroundColor.B));
                        }
                    }
                }
            }

            string strSaved = string.Format("{0}.png", GetFileOnlyName(GetFileName(strFileName)));
            NewBmp.Save(strSaved, System.Drawing.Imaging.ImageFormat.Png);            
        }

        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
                e.Effect = DragDropEffects.Copy;
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            var filenames = (string[])e.Data.GetData(DataFormats.FileDrop);
            if (filenames.Length >= 1)
            {
                string strFileName = filenames[0];

                if (GetFileExtName(strFileName).ToLower() == "png" || GetFileExtName(strFileName).ToLower() == "bmp" ||
                    GetFileExtName(strFileName).ToLower() == "gif" || GetFileExtName(strFileName).ToLower() == "jpg" )
                {
                    label1.Visible = false;
                    strPngName = strFileName;
                    pictureBox1.Image = Image.FromFile(strFileName);
                    SavePng(strFileName);                                
                    this.Close();
                }
            }
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            PictureBox pb = (PictureBox)sender;
            Bitmap bmp = new Bitmap(pb.Image);
            backgroundColor = bmp.GetPixel(e.X, e.Y);
            SavePng(strPngName);
            this.Close();
        }
    }
}
