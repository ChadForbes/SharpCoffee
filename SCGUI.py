import tkinter
from tkinter import *
from tkinter import scrolledtext
import os.path

#TODO: screen should resize nicer

#suplimental methods
#empty

JavaTextArea =""
CSharpTextArea=""
JavaSaveDir=""
CSharpName=""
CSharpSaveDir=""
JavaName=""

#methods for when buttons are pressed
#TODO: export C# code in text box to translate into java code which will display in the java textbox
def exportAction():
    pass

#TODO: take in C# file and put its content into the textbox
def importAction():
    importWindow = Tk()
    importWindow.geometry("500x100")
    importWindow.title("Import File")

    importDirLabel = Label(importWindow, text="File Route ")
    importDirLabel.grid(row=1, column=0, padx=10, pady=10)
    importDirEntry = tkinter.Entry(importWindow, width=50)
    importDirEntry.grid(row=1, column=1, padx=10, pady=10)
    pass

#TODO: send code to existing file
def SaveAction(Language):
    #https://stackoverflow.com/questions/8024248/telling-python-to-save-a-txt-file-to-a-certain-directory-on-windows-and-mac

    if Language == "Java":
        if(JavaSaveDir==""):
            return
        completeName =os.path.join(JavaSaveDir,JavaSaveName)
        file2Save = open(completeName, "w")
        file2Save.write(JavaTextArea.get(1.0,END))
    elif Language == "CSharp":
        if (CSharpSaveDir == ""):
            return
        completeName =os.path.join(JavaSaveDir,CSharpName)
        file2Save = open(completeName, "w")
        file2Save.write(CSharpTextArea.get(1.0, END))
    else:
        print("Error no language detected")
        return
    file2Save.close()

def SaveAsAction2(Language,Dir,Name):
    if Language=="Java":
        JavaSaveDir=Dir
        JavaSaveName=Name
    elif Language =="CSharp":
        CSharpSaveDir = Dir
        CSharpSaveName = Name
    else:
        print("Error no language detected")
    SaveAction(Language)


#TODO: send code to new file
def SaveAsAction(Language):
    #SaveAs screen set up
    saveAsWindow = Tk()
    saveAsWindow.geometry("500x150")
    saveAsWindow.title("Save "+Language+" file as ")

    saveNameLabel = Label(saveAsWindow,text="Save As: ")
    saveNameLabel.grid(row =0,column=0, padx=10, pady=10)
    saveNameEntry = tkinter.Entry(saveAsWindow,width = 50)
    saveNameEntry.grid(row =0,column=1, padx=10, pady=10)

    DirLabel = Label(saveAsWindow, text="To Directory: ")
    DirLabel.grid(row=1, column=0, padx=10, pady=10)
    DirEntry = tkinter.Entry(saveAsWindow,width =50)
    DirEntry.grid(row =1,column=1, padx=10, pady=10)

    SaveButton = Button(saveAsWindow, text="save", command=SaveAsAction2(Language,DirEntry.get(),saveNameEntry.get()))
    SaveButton.grid(row=2, column=1, padx=10, pady=10)

#variables used
#currentSaveName - default to None/null
#currentSaveLoc - default to None/null

#begining UI setup
parentWindow = Tk()
parentWindow.title("C# to Java Cross-compiler")
parentWindow.geometry("950x500")

#parentWindow.grid_columnconfigure(0,weight=2)
parentWindow.grid_columnconfigure(1,weight=1)#needed
#parentWindow.grid_columnconfigure(2,weight=1)
parentWindow.grid_columnconfigure(3,weight=1)#needed
#parentWindow.grid_rowconfigure(0,weight=1)
parentWindow.grid_rowconfigure(1,weight=1)#needed
#parentWindow.grid_rowconfigure(2,weight=1)
#parentWindow.grid_rowconfigure(3,weight=1)
#parentWindow.grid_rowconfigure(4,weight=1)

CSharpLabel = Label(parentWindow,text="C#")
CSharpLabel.grid(row=0, column=0, padx=10, pady=10)

CSharpTextArea = scrolledtext.ScrolledText(parentWindow,width=50,height=200)
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

JavaTextArea = scrolledtext.ScrolledText(parentWindow,width=50,height=200)
JavaTextArea.grid(row =1,column=3, padx=10, pady=10)

JavaSaveFrame = tkinter.Frame(parentWindow)
JavaSaveFrame.grid(row=3,column=3, padx=10, pady=10)
SaveJavaButton = Button(JavaSaveFrame, text ="save", command = SaveAction("Java"))
SaveJavaButton.grid(row=0,column=0, padx=10, pady=10)
SaveAsJavaButton = Button(JavaSaveFrame, text ="save as", command = SaveAsAction("Java"))
SaveAsJavaButton.grid(row=0,column=1, padx=10, pady=10)

#this makes sure the GUI stays up
parentWindow.mainloop()