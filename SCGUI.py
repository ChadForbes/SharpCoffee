import tkinter
from tkinter import *
from tkinter import scrolledtext
from tkinter import filedialog as tkFileDialog
from PIL import Image, ImageTk
#from tkinter import tkFileDiaglog
#import os.pat

#TODO: screen should resize nicer

#suplimental methods
#empty

JavaTextArea =""
CSharpTextArea=""
JavaSaveDir=""
CSharpSaveDir=""
CurrLanguage = ""#used for SaveAs ing

#methods for when buttons are pressed
#exports C# code in text box to translate into java code which will display in the java textbox
def exportAction():
    #z_Result = subprocess.run(['SharpCoffeeApp.exe',<Path to C# file>,<Input language>,<Output language>],text=True, capture_output=True)
    if CSharpSaveDir=="":#makes sure a file to be translated is saved if not prompt the user to do so
        CSharpSaveAsAction()
    else:
        z_Result = subprocess.run(['SharpCoffeeApp.exe', CSharpSaveDir,"CSharp","Java"],text=True, capture_output=True)
        resultString = z_Result.stdout
        JavaTextArea.insert(INSERT,returnString)

#takes in C# file and put its content into the textbox
def CSharpImportAction():
    # note even if your importing a file you must save through the application, this is an intentional design choice we wouldnt want users accidently deleting something through the app
    importedCSharpFile = tkFileDialog.askopenfilename()
    print(importedCSharpFile)
    filelines = open(importedCSharpFile)
    for line in filelines:
        print(line)
        CSharpTextArea.insert(INSERT,line)

#general save function done after doing language specific things
def SaveAsAction():
    global JavaSaveDir
    global CSharpSaveDir
    f = tkFileDialog.asksaveasfile(mode = 'w', defaultextension = ".txt")
    if f is None:
        return
    #savedir = f.name
    f.write(text2save)
    if CurrLanguage=="Java":
         JavaSaveDir = f.name
    elif CurrLanguage=="CSharp":
        CSharpSaveDir = f.name
    f.close()
    pass


#variables used
#currentSaveName - default to None/null
#currentSaveLoc - default to None/null

#asks for a location to save contents of Java text box and then does so
def JavaSaveAsAction():
    global CurrLanguage
    CurrLanguage = "Java"
    global text2save
    text2save=JavaTextArea.get("1.0",END)
    SaveAsAction()
#asks for a location to save contents of CSharp text box and then does so
def CSharpSaveAsAction():
    global CurrLanguage
    CurrLanguage = "CSharp"
    global text2save
    text2save=CSharpTextArea.get("1.0",END)
    SaveAsAction()
#saves contents of Java text box using directory it was "savesAs" with
def JavaSaveAction():
    if JavaSaveDir == "":  # if the Directory is not there we open saveAS
        JavaSaveAsAction()
    else:
        file2Save = open(JavaSaveDir, "w")
        file2Save.write(JavaTextArea.get("1.0", END))
        file2Save.close()
#saves contents of CSharp text box using directory it was "savesAs" with
def CSharpSaveAction():
    if CSharpSaveDir == "":  # if the Directory is not there we open saveAS
        CSharpSaveAsAction()
    else:
        file2Save = open(CSharpSaveDir, "w")
        file2Save.write(CSharpTextArea.get("1.0", END))
        file2Save.close()

#begining UI setup
parentWindow = Tk()
parentWindow.title("C# to Java Cross-compiler")
parentWindow.geometry("1200x500")

image = PhotoImage(file ="SharpCoffeeLogo.PNG") #having the image in the background looks bad so im judt going it like this
x = Label (image = image)
x.grid(row = 1, column = 2)

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

ImportButton = Button(parentWindow,text = "import", command = CSharpImportAction)
ImportButton.grid(row=3,column=0, padx=10, pady=10)

CSharpSaveFrame = tkinter.Frame(parentWindow)
CSharpSaveFrame.grid(row=4,column=0)#removed padding to make it look nicer imo
SaveCSharpButton = Button(CSharpSaveFrame, text="save", command = CSharpSaveAction)
SaveCSharpButton.grid(row=0,column=0, padx=10, pady=10)
SaveAsCSharpButton = Button(CSharpSaveFrame, text="save as", command = CSharpSaveAsAction)
SaveAsCSharpButton.grid(row=0,column=1, padx=10, pady=10)

ExportButton = Button(parentWindow,text = "convert", command = exportAction)
ExportButton.grid(row=3,column=2, padx=10, pady=10)

JavaLabel = Label(parentWindow,text="Java")
JavaLabel.grid(row=0, column=3, padx=10, pady=10)

JavaTextArea = scrolledtext.ScrolledText(parentWindow,width=50,height=200)
JavaTextArea.grid(row =1,column=3, padx=10, pady=10)

JavaSaveFrame = tkinter.Frame(parentWindow)
JavaSaveFrame.grid(row=3,column=3, padx=10, pady=10)
SaveJavaButton = Button(JavaSaveFrame, text ="save", command = JavaSaveAction)
SaveJavaButton.grid(row=0,column=0, padx=10, pady=10)
SaveAsJavaButton = Button(JavaSaveFrame, text ="save as", command = JavaSaveAsAction)
SaveAsJavaButton.grid(row=0,column=1, padx=10, pady=10)


#this makes sure the GUI stays up
parentWindow.mainloop()