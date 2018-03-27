startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataDone/file_search_time.txt", header = TRUE)


plot(log(time)~files, data = startup)
plot(log(time)~tags, data = startup)
plot(log(time)~log(answerfiles), data = startup)

startup<-startup[startup$time < .01,]
fit<-lm(log(time)~log(answerfiles) + I(log(answerfiles)^2), data = startup)
summary(fit)
plot(fit)

plot(fit$residuals~log(startup$answerfiles),ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
