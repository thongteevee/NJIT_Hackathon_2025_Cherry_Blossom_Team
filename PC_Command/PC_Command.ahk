SoundSet, 0  ; Mute master volume

websites := []
websites.Push("https://www.youtube.com/watch?v=jsQXgDZIIrY")
websites.Push("https://youtu.be/p_di4Zn4wz4?si=SzooNJGQYnN04Bw_&t=630")
websites.Push("https://njit.instructure.com/courses/44333/files/7668577?module_item_id=1637330")

; Randomly choose between: 1 = website, 2 = Paint, 3 = PDF
Random, choice, 1, 3

if (choice = 1) {
    Random, idx, 1, % websites.Length()
    Run, % websites[idx]
} else if (choice = 2) {
    ;Run, mspaint.exe
} else if (choice = 3) {
    Run, "C:\Users\Thong Khong\Downloads\ECE251 Student Notes ver6.pdf"
}