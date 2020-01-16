import tkinter
from tkinter import *
from tkinter import scrolledtext

#methods for when buttons are pressed
#TODO: export C# code in text box to translate into java code which will display in the java textbox
def exportAction():
    pass

#TODO: take in C# file and put its content into the textbox
def importAction():
    pass

#TODO: send code to existing file
def SaveAction(Language):
    pass

#TODO: send code to new file
def SaveAsAction(Language):
    pass

#begining UI setup
parentWindow = Tk()
parentWindow.title("C# to Java Cross-compiler")
parentWindow.geometry("950x500")

CSharpLabel = Label(parentWindow,text="C#")
CSharpLabel.grid(row=0, column=0, padx=10, pady=10)

CSharpTextArea = scrolledtext.ScrolledText(parentWindow,width=50,height=20)
CSharpTextArea.grid(row =1,column=0, padx=10, pady=10)

ImportButton = Button(parentWindow,text = "import", command = importAction)
ImportButton.grid(row=3,column=0, padx=10, pady=10)

CSharpSaveFrame = tkinter.Frame(parentWindow)
CSharpSaveFrame.grid(row=4,column=0)#removed padding to make it look nicer imo
SaveCSharpButton = Button(CSharpSaveFrame, text="save", command = SaveAction("CSharp"))
SaveCSharpButton.grid(row=0,column=0, padx=10, pady=10)
SaveAsCSharpButton = Button(CSharpSaveFrame, text="save as", command = SaveAsAction("CSharp"))
SaveAsCSharpButton.grid(row=0,column=1, padx=10, pady=10)

ExportButton = Button(parentWindow,text = "export", command = exportAction)
ExportButton.grid(row=3,column=2, padx=10, pady=10)

JavaLabel = Label(parentWindow,text="Java")
JavaLabel.grid(row=0, column=3, padx=10, pady=10)

JavaTextArea = scrolledtext.ScrolledText(parentWindow,width=50,height=20)
JavaTextArea.grid(row =1,column=3, padx=10, pady=10)

JavaSaveFrame = tkinter.Frame(parentWindow)
JavaSaveFrame.grid(row=3,column=3, padx=10, pady=10)
SaveJavaButton = Button(JavaSaveFrame, text ="save", command = SaveAction("Java"))
SaveJavaButton.grid(row=0,column=0, padx=10, pady=10)
SaveAsJavaButton = Button(JavaSaveFrame, text ="save as", command = SaveAsAction("Java"))
SaveAsJavaButton.grid(row=0,column=1, padx=10, pady=10)

#this makes sure the GUI stays up
parentWindow.mainloop()