startup<-read.table("/home/wyatt/Documents/Arboreal/Source/DataNormal/file_search_time.txt", header = TRUE)

startup<-startup[startup$time < 1,]
plot(log(time)~files, data = startup)
plot(log(time)~tags, data = startup)
plot(log(time)~log(answerfiles), data = startup)

fit<-lm(log(time)~log(answerfiles) + I(log(answerfiles)^2) + answerfiles*tags*files - answerfiles:tags:files - answerfiles:tags - tags:files + log(files) - files, data = startup)
summary(fit)
plot(fit)

plot(fit$residuals~startup$tags,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$answerfiles,ylab="Residuals", xlab="Files", main="Files Residual Plot")
plot(fit$residuals~startup$files,ylab="Residuals", xlab="Files", main="Files Residual Plot")
