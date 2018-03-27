startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataDone/tag_file_time.txt", header = TRUE)


plot(time~files, data = startup)
plot(time~tags, data = startup)
plot(time~numtags, data = startup)


fit<-lm(time~I(numtags^2), data = startup)
summary(fit)
plot(fit)


plot(fit$residuals~startup$numtags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
