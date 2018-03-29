startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataNormal/tag_file_time.txt", header = TRUE)


plot(log(time)~files, data = startup)
plot(log(time)~tags, data = startup)
plot(log(time)~numtags, data = startup)


fit<-lm(log(time)~files + tags + numtags, data = startup)
summary(fit)
plot(fit)


plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$numtags,ylab="Residuals", xlab="Files", main="Files Residual Plot")

