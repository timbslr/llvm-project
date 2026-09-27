target datalayout = "e-m:e-p:16:16-i8:8-i16:16-a:8-n8"
target triple = "sebos-unknown-elf"

define i8 @sum_to_n(i8 %n) {
entry:
  br label %loop

loop:
  %i = phi i8 [ 0, %entry ], [ %i.next, %loop ]
  %acc = phi i8 [ 0, %entry ], [ %acc.next, %loop ]
  %acc.next = add i8 %acc, %i
  %i.next = add i8 %i, 1
  %cond = icmp ult i8 %i.next, %n
  br i1 %cond, label %loop, label %exit

exit:
  ret i8 %acc.next
}